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

	ComPtr<IWICBitmapDecoder> pDecoder;
	ComPtr<IWICBitmapFrameDecode> pFrameEncode;
	HRESULT hr = _renderer->getWICFactory()->CreateDecoderFromFilename(pBitmap->getFileName().c_str(), nullptr,
		GENERIC_READ, WICDecodeMetadataCacheOnLoad, &pDecoder);

	if (SUCCEEDED(hr))
		hr = pDecoder->GetFrame(0, &pFrameEncode);

	if (SUCCEEDED(hr))
		hr = _renderer->getWICFactory()->CreateFormatConverter(&pBitmap->_wicConverter);

	if (SUCCEEDED(hr))
	{
		UINT originalWidth, originalHeight;
		hr = pFrameEncode->GetSize(&originalWidth, &originalHeight);

		if (width != 0 || height != 0)
		{
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
					hr = pBitmap->_wicConverter->Initialize(pScaler.Get(), GUID_WICPixelFormat32bppPBGRA,
						WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeMedianCut);

				if (SUCCEEDED(hr))
					hr = _renderer->getWICFactory()->CreateBitmapFromSourceRect(
						pScaler.Get(), 0, 0, width, height, &pBitmap->_wicBitmap);
			}
		}
		else
		{
			if (SUCCEEDED(hr))
				hr = pBitmap->_wicConverter->Initialize(pFrameEncode.Get(), GUID_WICPixelFormat32bppPBGRA,
					WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeMedianCut);

			if (SUCCEEDED(hr))
				hr = _renderer->getWICFactory()->CreateBitmapFromSourceRect(
					pFrameEncode.Get(), 0, 0, originalWidth, originalHeight, &pBitmap->_wicBitmap);
		}
	}

	if (SUCCEEDED(hr))
		pBitmap->cleanPic();

	printComError(hr);
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
		wstring hint = L"创建文件对象失败!";
		hint += pBitmap->getFileName().c_str();
		printLastError(hint);
		return false;
	}

	HANDLE hFileMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, 0, NULL);
	if (hFileMap == NULL)
	{
		CloseHandle(hFile);
		wstring hint = L"创建文件映射对象失败!";
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
			wstring hint = L"映射文件视图失败!";
			hint += pBitmap->getFileName().c_str();
			printLastError(hint);
			return false;
		}

		if (SUCCEEDED(hr))
			hr = pStream->InitializeFromMemory(lpbMapAddress, dwBlockBytes);

		ComPtr<IWICBitmapDecoder> pDecoder;
		if (SUCCEEDED(hr))
			hr = _renderer->getWICFactory()->CreateDecoderFromStream(pStream.Get(), nullptr,
				WICDecodeMetadataCacheOnDemand, &pDecoder);

		ComPtr<IWICBitmapFrameDecode> pFrameEncode;
		if (SUCCEEDED(hr))
			hr = pDecoder->GetFrame(0, &pFrameEncode);

		if (SUCCEEDED(hr))
			hr = _renderer->getWICFactory()->CreateFormatConverter(&pBitmap->_wicConverter);

		if (SUCCEEDED(hr))
		{
			UINT originalWidth, originalHeight;
			hr = pFrameEncode->GetSize(&originalWidth, &originalHeight);

			if (width != 0 || height != 0)
			{
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
						hr = pBitmap->_wicConverter->Initialize(pScaler.Get(), GUID_WICPixelFormat32bppPBGRA,
							WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeMedianCut);

					if (SUCCEEDED(hr))
						hr = _renderer->getWICFactory()->CreateBitmapFromSourceRect(
							pScaler.Get(), 0, 0, width, height, &pBitmap->_wicBitmap);
				}
			}
			else
			{
				if (SUCCEEDED(hr))
					hr = pBitmap->_wicConverter->Initialize(pFrameEncode.Get(), GUID_WICPixelFormat32bppPBGRA,
						WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeMedianCut);

				if (SUCCEEDED(hr))
					hr = _renderer->getWICFactory()->CreateBitmapFromSourceRect(
						pFrameEncode.Get(), 0, 0, originalWidth, originalHeight, &pBitmap->_wicBitmap);
			}
		}

		if (SUCCEEDED(hr))
			pBitmap->cleanPic();

		UnmapViewOfFile(lpbMapAddress);

		if (FAILED(hr))
			break;

		qwFileOffset += dwBlockBytes;
		qwFileSize -= dwBlockBytes;
	}

	CloseHandle(hFileMap);

	printComError(hr);
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
	{
		D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties();
		rtProps.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);
		rtProps.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
		rtProps.usage = D2D1_RENDER_TARGET_USAGE_NONE;
		hr = _renderer->getD2DFactory()->CreateWicBitmapRenderTarget(pWicBitmap.Get(), &rtProps, &pD2DRenderTarget);
	}

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

	printComError(hr);
	return SUCCEEDED(hr);
}

bool D2DPicProcessor::blend(PicturePtr picDst, const PicturePtr picSrc, Color clr)
{
	D2DBitmap* pBitmapSrc = dynamic_cast<D2DBitmap*>(picSrc.get());
	D2DBitmap* pBitmapDst = dynamic_cast<D2DBitmap*>(picDst.get());
	if (!pBitmapSrc || !pBitmapDst || !pBitmapSrc->_wicBitmap)
		return false;

	UINT originalWidth = 0, originalHeight = 0;
	HRESULT hr = pBitmapSrc->_wicBitmap->GetSize(&originalWidth, &originalHeight);

	if (SUCCEEDED(hr))
	{
		ComPtr<IWICBitmapLock> pILock;
		WICRect rcLock = { 0, 0, originalWidth, originalHeight };
		hr = pBitmapSrc->_wicBitmap->Lock(&rcLock, WICBitmapLockWrite, &pILock);

		if (SUCCEEDED(hr))
		{
			UINT cbBufferSize = 0;
			BYTE* pv = nullptr;
			hr = pILock->GetDataPointer(&cbBufferSize, &pv);
			if (SUCCEEDED(hr))
			{
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
	}

	if (SUCCEEDED(hr))
		hr = _renderer->getWICFactory()->CreateFormatConverter(&pBitmapDst->_wicConverter);

	ComPtr<IWICBitmapScaler> pScaler;
	if (SUCCEEDED(hr))
		hr = _renderer->getWICFactory()->CreateBitmapScaler(&pScaler);

	if (SUCCEEDED(hr))
		hr = pScaler->Initialize(pBitmapSrc->_wicBitmap.Get(), originalWidth, originalHeight, WICBitmapInterpolationModeCubic);

	if (SUCCEEDED(hr))
		hr = pBitmapDst->_wicConverter->Initialize(pScaler.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone,
			nullptr, 0.0f, WICBitmapPaletteTypeMedianCut);

	if (SUCCEEDED(hr))
		hr = _renderer->getWICFactory()->CreateBitmapFromSourceRect(pScaler.Get(), 0, 0, originalWidth, originalHeight,
			&pBitmapDst->_wicBitmap);

	if (SUCCEEDED(hr))
		pBitmapDst->cleanPic();

	printComError(hr);
	return SUCCEEDED(hr);
}

bool D2DPicProcessor::blend(PicturePtr picDst, const PicturePtr picBelow, const PicturePtr picAbove, float proportion)
{
	D2DBitmap* pBitmapBelow = dynamic_cast<D2DBitmap*>(picBelow.get());
	D2DBitmap* pBitmapAbove = dynamic_cast<D2DBitmap*>(picAbove.get());
	D2DBitmap* pBitmapDst = dynamic_cast<D2DBitmap*>(picDst.get());
	if (!pBitmapBelow || !pBitmapAbove || !pBitmapDst || !pBitmapBelow->_wicBitmap || !pBitmapAbove->_wicBitmap)
		return false;

	UINT originalWidth = 0, originalHeight = 0, width = 0, height = 0;

	HRESULT hr = pBitmapBelow->_wicBitmap->GetSize(&originalWidth, &originalHeight);

	if (SUCCEEDED(hr))
		hr = pBitmapAbove->_wicBitmap->GetSize(&width, &height);

	ComPtr<IWICBitmap> pWicBitmap;
	if (originalWidth != width || originalHeight != height)
	{
		ComPtr<IWICBitmapScaler> pScaler;
		if (SUCCEEDED(hr))
			hr = _renderer->getWICFactory()->CreateBitmapScaler(&pScaler);

		if (SUCCEEDED(hr))
			hr = pScaler->Initialize(pBitmapAbove->_wicBitmap.Get(), originalWidth, originalHeight, WICBitmapInterpolationModeCubic);

		if (SUCCEEDED(hr))
			hr = _renderer->getWICFactory()->CreateBitmapFromSourceRect(pScaler.Get(), 0, 0, originalWidth, originalHeight,
				&pWicBitmap);
	}
	else
	{
		pWicBitmap = pBitmapAbove->_wicBitmap;
	}

	UINT cbBufferSizeAbove = 0;
	BYTE* pvAbove = nullptr;

	if (SUCCEEDED(hr))
	{
		ComPtr<IWICBitmapLock> pILock;
		WICRect rcLock = { 0, 0, originalWidth, originalHeight };
		hr = pWicBitmap->Lock(&rcLock, WICBitmapLockWrite, &pILock);

		if (SUCCEEDED(hr))
			hr = pILock->GetDataPointer(&cbBufferSizeAbove, &pvAbove);
	}

	if (SUCCEEDED(hr))
	{
		ComPtr<IWICBitmapLock> pILock;
		WICRect rcLock = { 0, 0, originalWidth, originalHeight };
		hr = pBitmapBelow->_wicBitmap->Lock(&rcLock, WICBitmapLockWrite, &pILock);

		if (SUCCEEDED(hr))
		{
			UINT cbBufferSizeBelow = 0;
			BYTE* pvBelow = nullptr;
			hr = pILock->GetDataPointer(&cbBufferSizeBelow, &pvBelow);
			if (SUCCEEDED(hr))
			{
				for (unsigned int i = 0; i < cbBufferSizeBelow; i += 4)
				{
					if (pvAbove[i + 3] != 0)
					{
						pvBelow[i] = pvBelow[i] * (1 - proportion) + pvAbove[i] * proportion;
						pvBelow[i + 1] = pvBelow[i + 1] * (1 - proportion) + pvAbove[i + 1] * proportion;
						pvBelow[i + 2] = pvBelow[i + 2] * (1 - proportion) + pvAbove[i + 2] * proportion;
						pvBelow[i + 3] = pvBelow[i + 3] * (1 - proportion) + pvAbove[i + 3] * proportion;
					}
				}
			}
		}
	}

	if (SUCCEEDED(hr))
		hr = _renderer->getWICFactory()->CreateFormatConverter(&pBitmapDst->_wicConverter);

	ComPtr<IWICBitmapScaler> pScaler;
	if (SUCCEEDED(hr))
		hr = _renderer->getWICFactory()->CreateBitmapScaler(&pScaler);

	if (SUCCEEDED(hr))
		hr = pScaler->Initialize(pBitmapBelow->_wicBitmap.Get(), originalWidth, originalHeight, WICBitmapInterpolationModeCubic);

	if (SUCCEEDED(hr))
		hr = pBitmapDst->_wicConverter->Initialize(pScaler.Get(), GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeMedianCut);

	if (SUCCEEDED(hr))
		hr = _renderer->getWICFactory()->CreateBitmapFromSourceRect(pScaler.Get(), 0, 0, originalWidth, originalHeight,
			&pBitmapDst->_wicBitmap);

	if (SUCCEEDED(hr))
		pBitmapDst->cleanPic();

	printComError(hr);
	return SUCCEEDED(hr);
}
