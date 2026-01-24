#include "stlrpch.h"
#include "MetalDevice.h"

namespace Stellar {
    MetalDevice* MetalDevice::s_Instance = nullptr;

    MetalDevice *MetalDevice::GetInstance() {
        if (s_Instance == nullptr)
            s_Instance = new MetalDevice();
        return s_Instance;
    }

    MetalDevice::~MetalDevice() {
        m_CommandQueue->release();
        m_Device->release();
    }

    void MetalDevice::init() {
        NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();
        m_Device = MTL::CreateSystemDefaultDevice();
        m_CommandQueue = m_Device->newCommandQueue();
        pool->release();
    }

    MTL::Device* MetalDevice::getDevice() {
        return m_Device;
    }

    MTL::CommandQueue *MetalDevice::getCommandQueue() {
        return m_CommandQueue;
    }
}