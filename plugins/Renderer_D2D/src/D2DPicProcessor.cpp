#include "D2DPicProcessor.h"
#include "D2DRenderer.h"
#include "D2DBitmap.h"
#include "VayoUtils.h"

D2DPicProcessor::D2DPicProcessor(D2DRenderer* renderer)
	: _renderer(renderer)
{
}

bool D2DPicProcessor::load(PicturePtr picDst, unsigned int width /*= 0*/, unsigned int height /*= 0*/)
{
	D2DBitmap* pBitmap = dynamic_cast<D2DBitmap*>(picDst.get());
	if (!pBitmap)
		return false;

	ComPtr<IWICBitmapFrameDecode> pFrameEncode;
	HRESULT hr = _renderer->getWICFactory()->CreateDecoderFromFilename(pBitmap->getFileName().c_str(), nullptr,
		GENERIC_READ, WICDecodeMetadataCacheOnLoad, &pBitmap->_decoder);

	if (SUCCEEDED(hr))
		hr = pBitmap->_decoder->GetFrame(0, &pFrameEncode);

	if (SUCCEEDED(hr))
		hr = _renderer->getWICFactory()->CreateFormatConverter(&pBitmap->_converter);

	if (SUCCEEDED(hr))
	{
		if (width != 0 || height != 0)
		{
			UINT originalWidth, originalHeight;
			hr = pFrameEncode->GetSize(&originalWidth, &originalHeight);
			if (SUCCEEDED(hr))
			{
				if (width == 0)
				{
					FLOAT scalar = static_cast<FLOAT>(height) / static_cast<FLOAT>(originalHeight);
					width = static_cast<UINT>(scalar * static_cast<FLOAT>(originalWidth));
				}
				else if (height == 0)
				{
					FLOAT scalar = static_cast<FLOAT>(width) / static_cast<FLOAT>(originalWidth);
					height = static_cast<UINT>(scalar * static_cast<FLOAT>(originalHeight));
				}

				ComPtr<IWICBitmapScaler> pScaler;
				hr = _renderer->getWICFactory()->CreateBitmapScaler(&pScaler);
				if (SUCCEEDED(hr))
					hr = pScaler->Initialize(pFrameEncode.Get(), width, height, WICBitmapInterpolationModeCubic);
				if (SUCCEEDED(hr))
					hr = pBitmap->_converter->Initialize(pScaler.Get(), GUID_WICPixelFormat32bppPBGRA,
						WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeMedianCut);
			}
		}
		else
		{
			hr = pBitmap->_converter->Initialize(pFrameEncode.Get(), GUID_WICPixelFormat32bppPBGRA,
				WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeMedianCut);
		}
	}

	if (SUCCEEDED(hr))
		pBitmap->cleanPic();

	return SUCCEEDED(hr);
}

bool D2DPicProcessor::loadByMmap(PicturePtr picDst, unsigned int width /*= 0*/, unsigned int height /*= 0*/)
{
	D2DBitmap* pBitmap = dynamic_cast<D2DBitmap*>(picDst.get());
	if (!pBitmap)
		return false;

	HANDLE hFile = CreateFile(pBitmap->getFileName().c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		wstring hint = L"创建文件对象失败:";
		hint += pBitmap->getFileName().c_str();
		printLastError(hint);
		return false;
	}

	HANDLE hFileMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, 0, NULL);
	if (hFileMap == NULL)
	{
		CloseHandle(hFile);
		wstring hint = L"创建文件映射对象失败:";
		hint += pBitmap->getFileName().c_str();
		printLastError(hint);
		return false;
	}

	SYSTEM_INFO SysInfo;
	GetSystemInfo(&SysInfo);
	DWORD dwGran = SysInfo.dwAllocationGranularity;

	DWORD dwFileSizeHigh;
	__int64 qwFileSize = GetFileSize(hFile, &dwFileSizeHigh);
	qwFileSize |= (((__int64)dwFileSizeHigh) << 32);

	CloseHandle(hFile);

	__int64 qwFileOffset = 0;

	DWORD dwBlockBytes = 1000 * dwGran;
	if (qwFileSize < 1000 * dwGran)
		dwBlockBytes = (DWORD)qwFileSize;

	ComPtr<IWICStream> pStream;
	HRESULT hr = _renderer->getWICFactory()->CreateStream(&pStream);
	if (FAILED(hr))
	{
		CloseHandle(hFileMap);
		return false;
	}

	while (qwFileSize > 0)
	{
		LPBYTE lpbMapAddress = (LPBYTE)MapViewOfFile(hFileMap, FILE_MAP_ALL_ACCESS,
			(DWORD)(qwFileOffset >> 32), (DWORD)(qwFileOffset & 0xFFFFFFFF), dwBlockBytes);

		if (lpbMapAddress == NULL)
		{
			wstring hint = L"映射文件视图失败:";
			hint += pBitmap->getFileName().c_str();
			printLastError(hint);
			return false;
		}

		if (SUCCEEDED(hr))
			hr = pStream->Write(lpbMapAddress, dwBlockBytes, nullptr);

		UnmapViewOfFile(lpbMapAddress);

		if (FAILED(hr))
			break;

		qwFileOffset += dwBlockBytes;
		qwFileSize -= dwBlockBytes;
	}

	CloseHandle(hFileMap);
	ComPtr<IWICBitmapFrameDecode> pFrameEncode;

	if (SUCCEEDED(hr))
		hr = _renderer->getWICFactory()->CreateDecoderFromStream(pStream.Get(), nullptr,
			WICDecodeMetadataCacheOnDemand, &pBitmap->_decoder);

	if (SUCCEEDED(hr))
		hr = pBitmap->_decoder->GetFrame(0, &pFrameEncode);

	if (SUCCEEDED(hr))
		hr = _renderer->getWICFactory()->CreateFormatConverter(&pBitmap->_converter);

	if (SUCCEEDED(hr))
	{
		if (width != 0 || height != 0)
		{
			UINT originalWidth, originalHeight;
			hr = pFrameEncode->GetSize(&originalWidth, &originalHeight);
			if (SUCCEEDED(hr))
			{
				if (width == 0)
				{
					FLOAT scalar = static_cast<FLOAT>(height) / static_cast<FLOAT>(originalHeight);
					width = static_cast<UINT>(scalar * static_cast<FLOAT>(originalWidth));
				}
				else if (height == 0)
				{
					FLOAT scalar = static_cast<FLOAT>(width) / static_cast<FLOAT>(originalWidth);
					height = static_cast<UINT>(scalar * static_cast<FLOAT>(originalHeight));
				}

				ComPtr<IWICBitmapScaler> pScaler;
				hr = _renderer->getWICFactory()->CreateBitmapScaler(&pScaler);
				if (SUCCEEDED(hr))
					hr = pScaler->Initialize(pFrameEncode.Get(), width, height, WICBitmapInterpolationModeCubic);
				if (SUCCEEDED(hr))
					hr = pBitmap->_converter->Initialize(pScaler.Get(), GUID_WICPixelFormat32bppPBGRA,
						WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeMedianCut);
			}
		}
		else
		{
			hr = pBitmap->_converter->Initialize(pFrameEncode.Get(), GUID_WICPixelFormat32bppPBGRA,
				WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeMedianCut);
		}
	}

	if (SUCCEEDED(hr))
		pBitmap->cleanPic();

	return SUCCEEDED(hr);
}

bool D2DPicProcessor::save(PicturePtr picDst, const PicturePtr picSrc)
{
	return save(picDst, picSrc, Rectf(0, 0, 0, 0));
}

bool D2DPicProcessor::save(PicturePtr picDst, const PicturePtr picSrc, const Rectf& dstRect)
{
	D2DBitmap* pBitmapSrc = dynamic_cast<D2DBitmap*>(picSrc.get());
	D2DBitmap* pBitmapDst = dynamic_cast<D2DBitmap*>(picDst.get());
	if (!pBitmapSrc || !pBitmapDst)
		return false;

	ComPtr<ID2D1Bitmap> bitmap = pBitmapSrc->getBitmap();
	if (!bitmap)
		return false;

	HRESULT hr = S_FALSE;
	ComPtr<IWICStream> pStream;
	ComPtr<IWICBitmap> pWicBitmap;
	ComPtr<IWICBitmapEncoder> pEncoder;
	ComPtr<IWICBitmapFrameEncode> pFrameEncode;
	ComPtr<ID2D1RenderTarget> pD2DRenderTarget;

	D2D1_SIZE_F sizeToSave = (dstRect.getWidth() <= 0 || dstRect.getHeight() <= 0)
		? bitmap->GetSize() : D2D1::SizeF(dstRect.getWidth(), dstRect.getHeight());

	hr = _renderer->getWICFactory()->CreateBitmap(sizeToSave.width, sizeToSave.height,
		GUID_WICPixelFormat32bppPBGRA, WICBitmapCacheOnLoad, &pWicBitmap);

	if (SUCCEEDED(hr))
		hr = _renderer->getD2DFactory()->CreateWicBitmapRenderTarget(pWicBitmap.Get(),
			&D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_SOFTWARE), &pD2DRenderTarget);

	if (SUCCEEDED(hr))
	{
		pD2DRenderTarget->BeginDraw();
		pD2DRenderTarget->DrawBitmap(bitmap.Get(),
			D2D1::RectF(dstRect._upperLeftCorner._x, dstRect._upperLeftCorner._y, sizeToSave.width, sizeToSave.height));
		hr = pD2DRenderTarget->EndDraw();
	}

	if (SUCCEEDED(hr))
		hr = _renderer->getWICFactory()->CreateStream(&pStream);

	if (SUCCEEDED(hr))
		hr = pStream->InitializeFromFilename(pBitmapDst->getFileName().c_str(), GENERIC_WRITE);

	if (SUCCEEDED(hr))
		hr = _renderer->getWICFactory()->CreateEncoder(GUID_ContainerFormatPng, nullptr, &pEncoder);

	if (SUCCEEDED(hr))
		hr = pEncoder->Initialize(pStream.Get(), WICBitmapEncoderNoCache);

	if (SUCCEEDED(hr))
		hr = pEncoder->CreateNewFrame(&pFrameEncode, nullptr);

	if (SUCCEEDED(hr))
		hr = pFrameEncode->Initialize(nullptr);

	if (SUCCEEDED(hr))
		hr = pFrameEncode->SetSize(sizeToSave.width, sizeToSave.height);

	if (SUCCEEDED(hr))
	{
		WICPixelFormatGUID format = GUID_WICPixelFormatDontCare;
		hr = pFrameEncode->SetPixelFormat(&format);
	}

	if (SUCCEEDED(hr))
		hr = pFrameEncode->WriteSource(pWicBitmap.Get(), nullptr);

	if (SUCCEEDED(hr))
		hr = pFrameEncode->Commit();

	if (SUCCEEDED(hr))
		hr = pEncoder->Commit();

	if (SUCCEEDED(hr))
		pBitmapDst->cleanPic();

	return SUCCEEDED(hr);
}

bool D2DPicProcessor::blend(PicturePtr picDst, const PicturePtr picSrc, Color clr)
{
	D2DBitmap* pBitmapSrc = dynamic_cast<D2DBitmap*>(picSrc.get());
	D2DBitmap* pBitmapDst = dynamic_cast<D2DBitmap*>(picDst.get());
	if (!pBitmapSrc || !pBitmapDst || !pBitmapSrc->_decoder)
		return false;

	ComPtr<IWICBitmap> pWicBitmap;
	ComPtr<IWICBitmapFrameDecode> pFrameEncode;
	UINT originalWidth = 0, originalHeight = 0;

	HRESULT hr = pBitmapSrc->_decoder->GetFrame(0, &pFrameEncode);

	if (SUCCEEDED(hr))
		hr = pFrameEncode->GetSize(&originalWidth, &originalHeight);

	if (SUCCEEDED(hr))
		hr = _renderer->getWICFactory()->CreateBitmapFromSourceRect(pFrameEncode.Get(),
			0, 0, (UINT)originalWidth, (UINT)originalHeight, &pWicBitmap);

	if (SUCCEEDED(hr))
	{
		ComPtr<IWICBitmapLock> pILock;
		WICRect rcLock = { 0, 0, originalWidth, originalHeight };
		hr = pWicBitmap->Lock(&rcLock, WICBitmapLockWrite, &pILock);

		if (SUCCEEDED(hr))
		{
			UINT cbBufferSize = 0;
			BYTE* pv = NULL;

			if (SUCCEEDED(hr))
				hr = pILock->GetDataPointer(&cbBufferSize, &pv);

			for (unsigned int i = 0; i < cbBufferSize; i += 4)
			{
				if (pv[i + 3] != 0)
				{
					pv[i] *= clr._b;
					pv[i + 1] *= clr._g;
					pv[i + 2] *= clr._r;
					pv[i + 3] *= clr._a;
				}
			}
		}
	}

	if (SUCCEEDED(hr))
		hr = _renderer->getWICFactory()->CreateFormatConverter(&pBitmapDst->_converter);

	ComPtr<IWICBitmapScaler> pScaler;
	if (SUCCEEDED(hr))
		hr = _renderer->getWICFactory()->CreateBitmapScaler(&pScaler);

	if (SUCCEEDED(hr))
		hr = pScaler->Initialize(pWicBitmap.Get(), originalWidth, originalHeight, WICBitmapInterpolationModeCubic);

	if (SUCCEEDED(hr))
		hr = pBitmapDst->_converter->Initialize(pScaler.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone,
			nullptr, 0.0f, WICBitmapPaletteTypeMedianCut);

	if (SUCCEEDED(hr))
		pBitmapDst->cleanPic();

	return SUCCEEDED(hr);
}

bool D2DPicProcessor::blend(PicturePtr picDst, const PicturePtr picSrc1, const PicturePtr picSrc2, float proportion)
{
	D2DBitmap* pBitmapSrc1 = dynamic_cast<D2DBitmap*>(picSrc1.get());
	D2DBitmap* pBitmapSrc2 = dynamic_cast<D2DBitmap*>(picSrc2.get());
	D2DBitmap* pBitmapDst = dynamic_cast<D2DBitmap*>(picDst.get());
	if (!pBitmapSrc1 || !pBitmapSrc2 || !pBitmapDst || !pBitmapSrc1->_decoder || !pBitmapSrc2->_decoder)
		return false;

	ComPtr<IWICBitmap> pWicBitmapSrc1;
	ComPtr<IWICBitmap> pWicBitmapSrc2;
	ComPtr<IWICBitmapFrameDecode> pFrameEncodeSrc1;
	ComPtr<IWICBitmapFrameDecode> pFrameEncodeSrc2;
	UINT originalWidth = 0, originalHeight = 0;

	HRESULT hr = pBitmapSrc1->_decoder->GetFrame(0, &pFrameEncodeSrc1);

	if (SUCCEEDED(hr))
		hr = pFrameEncodeSrc1->GetSize(&originalWidth, &originalHeight);

	if (SUCCEEDED(hr))
		hr = _renderer->getWICFactory()->CreateBitmapFromSourceRect(
			pFrameEncodeSrc1.Get(), 0, 0, originalWidth, originalHeight, &pWicBitmapSrc1);

	UINT cbBufferSizeSrc = 0;
	BYTE* pvSrc = nullptr;

	if (SUCCEEDED(hr))
	{
		ComPtr<IWICBitmapLock> pILock;
		WICRect rcLock = { 0, 0, originalWidth, originalHeight };
		hr = pWicBitmapSrc1->Lock(&rcLock, WICBitmapLockWrite, &pILock);

		if (SUCCEEDED(hr))
			hr = pILock->GetDataPointer(&cbBufferSizeSrc, &pvSrc);
	}

	if (SUCCEEDED(hr))
		hr = pBitmapSrc2->_decoder->GetFrame(0, &pFrameEncodeSrc2);

	if (SUCCEEDED(hr))
		hr = pFrameEncodeSrc2->GetSize(&originalWidth, &originalHeight);

	if (SUCCEEDED(hr))
		hr = _renderer->getWICFactory()->CreateBitmapFromSourceRect(
			pFrameEncodeSrc2.Get(), 0, 0, originalWidth, originalHeight, &pWicBitmapSrc2);

	if (SUCCEEDED(hr))
	{
		ComPtr<IWICBitmapLock> pILock;
		WICRect rcLock = { 0, 0, originalWidth, originalHeight };
		hr = pWicBitmapSrc2->Lock(&rcLock, WICBitmapLockWrite, &pILock);

		if (SUCCEEDED(hr))
		{
			UINT cbBufferSize = 0;
			BYTE* pv = nullptr;

			if (SUCCEEDED(hr))
				hr = pILock->GetDataPointer(&cbBufferSize, &pv);

			for (unsigned int i = 0; i < cbBufferSize; i += 4)
			{
				if (pv[i + 3] != 0)
				{
					pv[i] = pv[i] * (1 - proportion) + pvSrc[i] * proportion;
					pv[i + 1] = pv[i + 1] * (1 - proportion) + pvSrc[i + 1] * proportion;
					pv[i + 2] = pv[i + 2] * (1 - proportion) + pvSrc[i + 2] * proportion;
					pv[i + 3] = pv[i + 3] * (1 - proportion) + pvSrc[i + 3] * proportion;
				}
			}
		}
	}

	if (SUCCEEDED(hr))
		hr = _renderer->getWICFactory()->CreateFormatConverter(&pBitmapDst->_converter);

	ComPtr<IWICBitmapScaler> pScaler;
	if (SUCCEEDED(hr))
		hr = _renderer->getWICFactory()->CreateBitmapScaler(&pScaler);

	if (SUCCEEDED(hr))
		hr = pScaler->Initialize(pWicBitmapSrc2.Get(), originalWidth, originalHeight, WICBitmapInterpolationModeCubic);

	if (SUCCEEDED(hr))
		hr = pBitmapDst->_converter->Initialize(pScaler.Get(), GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeMedianCut);

	if (SUCCEEDED(hr))
		pBitmapDst->cleanPic();

	return SUCCEEDED(hr);
}
