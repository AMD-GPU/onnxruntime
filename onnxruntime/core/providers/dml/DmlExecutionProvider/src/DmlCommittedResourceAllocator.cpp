// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "precomp.h"
#include "DmlCommittedResourceAllocator.h"
#include "DmlResourceWrapper.h"
#include "DmlCommittedResourceWrapper.h"

namespace Dml
{
    ComPtr<DmlResourceWrapper> DmlCommittedResourceAllocator::Alloc(size_t size, bool isUML)
    {
        ComPtr<ID3D12Resource> resource;

        D3D12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

        if (isUML)
        {
            heapProperties = {};
            heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;
            heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
            heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
            heapProperties.CreationNodeMask = 1;
            heapProperties.VisibleNodeMask = 1;
        }

        auto buffer = CD3DX12_RESOURCE_DESC::Buffer(size, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
        ORT_THROW_IF_FAILED(m_device->CreateCommittedResource(
            unmove_ptr(heapProperties),
            D3D12_HEAP_FLAG_NONE,
            &buffer,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_GRAPHICS_PPV_ARGS(resource.GetAddressOf())
        ));

        ComPtr<DmlResourceWrapper> resourceWrapper;
        wil::MakeOrThrow<DmlCommittedResourceWrapper>(std::move(resource)).As(&resourceWrapper);
        return resourceWrapper;
    }
}
