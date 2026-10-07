//
// GameBase.h
//

#pragma once

#include "GameBase.h"

struct DXTK_RESOURCE_STATES {
	D3D12_RESOURCE_STATES stateBefore;
	D3D12_RESOURCE_STATES stateAfter;
};

namespace TransitionState {
	extern const __declspec(selectany) DXTK_RESOURCE_STATES RenderTarget
	{ D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET };

	extern const __declspec(selectany) DXTK_RESOURCE_STATES Present
	{ D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT };
}

class dxtk final
{
public:
	~dxtk() = default;

	dxtk(dxtk&&) = default;
	dxtk& operator= (dxtk&&) = default;

	dxtk(dxtk const&) = delete;
	dxtk& operator= (dxtk const&) = delete;

	static dxtk* GetInstance()
	{
		static dxtk instance;
		return &instance;
	}

	inline ID3D12Device* GetD3D12Device() const { return Device; }
	inline ID3D12Device* GetD3D12Device(const int index) const { return m_gameBase->m_devices[index].Device.Get(); }

	//inline UINT        GetBackBufferIndex()   const { return m_gameBase->m_backBufferIndex;   }
	//inline DXGI_FORMAT GetBackBufferFormat()  const { return m_gameBase->m_backBufferFormat;  }
	//inline DXGI_FORMAT GetDepthBufferFormat() const { return m_gameBase->m_depthBufferFormat; }

	struct SWAPCHAIN final
	{
		friend class dxtk;

		const int            Width;
		const int            Height;

		DXGI_FORMAT          Format;
		DXGI_FORMAT          DepthFormat;

		const UINT           BufferCount;

		const D3D12_VIEWPORT Viewport;
		const RECT           OutputSize;

		SWAPCHAIN() :
			Width(static_cast<int>(screen::width)),
			Height(static_cast<int>(screen::height)),
			Format(DXGI_FORMAT_UNKNOWN), DepthFormat(DXGI_FORMAT_UNKNOWN),
			BufferCount(GameBase::c_swapBufferCount),
			Viewport{ 0.0f, 0.0f, FLOAT(screen::width), FLOAT(screen::height), D3D12_MIN_DEPTH, D3D12_MAX_DEPTH },
			OutputSize{ 0, 0, LONG(screen::width), LONG(screen::height) }
		{}

		inline UINT GetBufferIndex() const { return gameBase->m_backBufferIndex; }

		inline D3D12_CPU_DESCRIPTOR_HANDLE GetRTVDescriptorHeap() const noexcept
		{
			D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = gameBase->m_devices[PrimaryAdapter].RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			return std::move(cpuHandle);
		}

		inline D3D12_CPU_DESCRIPTOR_HANDLE GetDSVDescriptorHeap() const noexcept
		{
			D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = gameBase->m_dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			return std::move(cpuHandle);
		}

		__declspec(property(get = GetBufferIndex))		 UINT                        BufferIndex;
		__declspec(property(get = GetRTVDescriptorHeap)) D3D12_CPU_DESCRIPTOR_HANDLE RTVDescriptorHeap;
		__declspec(property(get = GetDSVDescriptorHeap)) D3D12_CPU_DESCRIPTOR_HANDLE DSVDescriptorHeap;

	private:
		GameBase* gameBase;
	} SwapChain;
	const SWAPCHAIN& BackBuffer = SwapChain;

	class COMMAND {
	public:
		friend class dxtk;

		COMMAND() = default;
		~COMMAND() = default;

		inline void Reset() const
		{ gameBase->ResetCommand(PrimaryAdapter); }

		inline void RTResourceBarrier(const DXTK_RESOURCE_STATES& states) const
		{ gameBase->RTResourceBarrier(PrimaryAdapter, states.stateBefore, states.stateAfter); }

		inline void RTSetupAndClear(const DirectX::XMVECTORF32& color) const
		{ gameBase->ClearRenderTarget(PrimaryAdapter, color); }

		inline void Prepare() const
		{
			gameBase->Prepare(PrimaryAdapter);
		}

		inline void Execute() const
		{
			DX::ThrowIfFailed(list->Close());
			queue->ExecuteCommandLists(1, CommandListCast(&list));
		}

		inline ID3D12CommandQueue* GetQueue() const { return queue; }
		__declspec(property(get = GetQueue)) ID3D12CommandQueue* Queue;

		inline ID3D12GraphicsCommandList* GetList() const { return list; }
		__declspec(property(get = GetList)) ID3D12GraphicsCommandList* List;

	private:
		GameBase*                  gameBase;
		ID3D12CommandQueue*        queue;
		ID3D12GraphicsCommandList* list;
	} Command;

	class AUDIO final {
	public:
		friend class dxtk;

		AUDIO() = default;
		~AUDIO() = default;

		inline void ResetEngine()
		{
			engine->Reset();
			*retry = false;
		}

		inline DirectX::AudioEngine* GetEngine() const { return engine; }
		__declspec(property(get = GetEngine)) DirectX::AudioEngine* Engine;

	private:
		DirectX::AudioEngine* engine;
		bool*                 retry;
	} Audio;

	class TIME final
	{
	public:
		friend class dxtk;

		float deltaTime = 0.0f;

		TIME() = default;
		~TIME() = default;

		//inline float GetDeltaTime() const noexcept { return float(timer->GetElapsedSeconds()); }
		inline float GetTime()      const noexcept { return float(timer->GetTotalSeconds()); }

		inline double GetDeltaTimeAsDobule() const noexcept { return timer->GetElapsedSeconds(); }
		inline double GetTimeAsDobule()      const noexcept { return timer->GetTotalSeconds(); }

		inline uint32_t GetFrameCount()      const noexcept { return timer->GetFrameCount(); }
		inline uint32_t GetFramesPerSecond() const noexcept { return timer->GetFramesPerSecond(); }

		//__declspec(property(get = GetDeltaTime))         float    deltaTime;
		__declspec(property(get = GetTime))              float    time;
		__declspec(property(get = GetDeltaTimeAsDobule)) double   deltaTimeAsDouble;
		__declspec(property(get = GetTimeAsDobule))      double   timeAsDouble;
		__declspec(property(get = GetFrameCount))        uint32_t frameCount;
		__declspec(property(get = GetFramesPerSecond))   uint32_t framePerSecond;

		inline void SetFixedFrameRate(const UINT fps) const noexcept
		{
			timer->SetFixedTimeStep(true);
			timer->SetTargetElapsedSeconds(1.0 / fps);
		}

		inline void SetVariableFrameRate() const noexcept { timer->SetFixedTimeStep(false); }

	private:
		DX::StepTimer* timer;
	} Time;

	inline void WaitForGpu() const { m_gameBase->WaitForGpu(); }

#if DXTK_MULTIGPU
	inline void Clear(const UINT adapter, const DirectX::XMVECTORF32& color)
	{ m_gameBase->Clear(adapter, color); }

	inline void ExecuteCommandList(const UINT adapter) const
	{ m_gameBase->ExecuteCommandList(adapter); }

	inline void ExecuteCommandLists(const UINT adapter, UINT NumCommandLists, ID3D12GraphicsCommandList* const* ppCommandList) const
	{ m_gameBase->ExecuteCommandLists(adapter, NumCommandLists, ppCommandList); }

	inline ID3D12Resource* PopulateCopyCommandList() { return m_gameBase->PopulateCopyCommandList(); }
	inline void ExecuteCopyCommandList() { m_gameBase->ExecuteCopyCommandList(); }

	inline void CommandQueueSignal(ID3D12CommandQueue* queue) { m_gameBase->CommandQueueSignal(queue); }
	inline void CommandQueueWait(ID3D12CommandQueue* queue) { m_gameBase->CommandQueueWait(queue); }

	inline void CrossAdapterQueueSignal() { m_gameBase->CrossAdapterQueueSignal(); }
	inline void CrossAdapterQueueWait() { m_gameBase->CrossAdapterQueueWait(); }

	inline void CreateCommandAllocator(const UINT adapter, ID3D12CommandAllocator** commandAllocator)
	{
		DX::ThrowIfFailed(m_gameBase->m_devices[adapter].Device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator)
		));
	}

	inline void CreateCommandList(
		const UINT adapter,
		ID3D12CommandAllocator* commandAllocator,
		ID3D12GraphicsCommandList** commandList)
	{
		DX::ThrowIfFailed(m_gameBase->m_devices[adapter].Device->CreateCommandList(
			0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, IID_PPV_ARGS(commandList)
		));
		DX::ThrowIfFailed((*commandList)->Close());
	}
#else
	//inline void ResetCommand() const
	//{
	//	m_gameBase->Prepare(PrimaryAdapter);
	//}

	//inline void ClearRenderTarget(const DirectX::XMVECTORF32& color) const
	//{
	//	m_gameBase->ClearRenderTarget(PrimaryAdapter, color);
	//}

	//inline void ExecuteCommandList() const
	//{
	//	m_gameBase->ExecuteCommandList(PrimaryAdapter);
	//}

	//inline void ExecuteCommandLists(UINT NumCommandLists, ID3D12GraphicsCommandList* const* ppCommandList) const
	//{
	//	m_gameBase->ExecuteCommandLists(PrimaryAdapter, NumCommandLists, ppCommandList);
	//}

	inline void BeginScene()
	{
		Command.Reset();
		Command.RTResourceBarrier(TransitionState::RenderTarget);
	}

	inline void EndScene()
	{
		Command.RTResourceBarrier(TransitionState::Present);
		Command.Execute();
	}

	inline void ClearRenderTarget(const DirectX::XMVECTORF32& color) const
	{
		m_gameBase->ClearRenderTarget(PrimaryAdapter, color);
	}

	inline void CreateCommandAllocator(ID3D12CommandAllocator** commandAllocator)
	{
		DX::ThrowIfFailed(m_gameBase->m_devices[PrimaryAdapter].Device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator)
		));
	}

	inline void CreateCommandList(
		ID3D12CommandAllocator* commandAllocator,
		ID3D12GraphicsCommandList** commandList
	)
	{
		DX::ThrowIfFailed(m_gameBase->m_devices[PrimaryAdapter].Device->CreateCommandList(
			0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, IID_PPV_ARGS(commandList)
		));
		DX::ThrowIfFailed((*commandList)->Close());
	}
#endif



#if DXTK_D3D11 || DXTK_DWRITE
	inline void CreateD3D11Device() const
	{ m_gameBase->D3D11CreateDevice(); }

	inline void Direct2DBegin()
	{
		m_gameBase->m_d3d11on12Device->AcquireWrappedResources(
			m_gameBase->m_wrappedBackBuffers[m_gameBase->m_backBufferIndex].GetAddressOf(), 1
		);
		m_gameBase->m_d2dDeviceContext->SetTarget(
			m_gameBase->m_d2dRenderTargets[m_gameBase->m_backBufferIndex].Get()
		);
		m_gameBase->m_d2dDeviceContext->BeginDraw();
	}

	inline void Direct2DEnd()
	{
		m_gameBase->m_d2dDeviceContext->EndDraw();
		m_gameBase->m_d3d11on12Device->ReleaseWrappedResources(
			m_gameBase->m_wrappedBackBuffers[m_gameBase->m_backBufferIndex].GetAddressOf(), 1
		);
		m_gameBase->m_d3d11DeviceContext->Flush();
	}
#endif

#if DXTK_D3D9
	inline void CreateD3D9Device() const { m_gameBase->D3D9CreateDevice();  }
#endif

	//__declspec(property(get = GetBackBufferIndex  )) UINT        BackBufferIndex;
	//__declspec(property(get = GetBackBufferFormat )) DXGI_FORMAT BackBufferFormat;
	//__declspec(property(get = GetDepthBufferFormat)) DXGI_FORMAT DepthBufferFormat;

public:
	//UINT                                     BackBufferCount       = GameBase::c_swapBufferCount;

#if DXTK_MULTIGPU
	ID3D12Device*                            PrimaryDevice         = nullptr;
	ID3D12CommandQueue*                      PrimaryCommandQueue   = nullptr;
	ID3D12GraphicsCommandList*               PrimaryCommandList    = nullptr;

	ID3D12Device*                            SecondaryDevice       = nullptr;
	ID3D12CommandQueue*                      SecondaryCommandQueue = nullptr;
	ID3D12GraphicsCommandList*               SecondaryCommandList  = nullptr;
#else
	ID3D12Device*                            Device                = nullptr;
	ID3D12CommandQueue*                      CommandQueue          = nullptr;
	ID3D12GraphicsCommandList*               CommandList           = nullptr;

	ID3D12Device*                            ComputeDevice         = nullptr;
	ID3D12CommandQueue*                      ComputeCommandQueue   = nullptr;
	ID3D12GraphicsCommandList*               ComputeCommandList    = nullptr;
#endif
	std::vector<std::unique_ptr<DirectX::DescriptorHeap>> DescriptorHeaps;

	inline DirectX::DescriptorHeap* GetDescriptorHeap() const { return DescriptorHeaps[0].get(); }
	inline void SetDescriptorHeap(DirectX::DescriptorHeap* descriptorHeap)
	{ DescriptorHeaps[0] = std::unique_ptr<DirectX::DescriptorHeap>(descriptorHeap); }
	__declspec(property(get = GetDescriptorHeap, put = SetDescriptorHeap)) DirectX::DescriptorHeap* DescriptorHeap;

#if DXTK_D3D11 || DXTK_DWRITE
	ID3D11Device*                            Device11              = nullptr;
	ID2D1DeviceContext2*                     Direct2D              = nullptr;
	IDWriteFactory*                          DirectWrite           = nullptr;
#endif
#if DXTK_D3D9
	IDirect3DDevice9*                        Device9               = nullptr;

	#include "Direct3D9.inc"
	direct3d9*                               Direct3D9             = nullptr;
#endif

	DirectX::AudioEngine*                    AudioEngine           = nullptr;

//#if DXTK_AMP
//	std::unique_ptr<concurrency::accelerator_view> PrimaryAMP;
//	std::unique_ptr<concurrency::accelerator_view> SecondaryAMP;
//#endif

private:
	friend class GameBase;

	dxtk() = default;

	inline void Attach(GameBase* const __restrict gameBase) noexcept
	{
		m_gameBase            = gameBase;

		SwapChain.gameBase    = gameBase;
		SwapChain.Format      = gameBase->m_backBufferFormat;
		SwapChain.DepthFormat = gameBase->m_depthBufferFormat;

		Time.timer            =&gameBase->m_timer;

#if DXTK_MULTIGPU
		PrimaryDevice         = gameBase->m_devices[0].Device.Get();
		PrimaryCommandQueue   = gameBase->m_devices[0].CommandQueue.Get();
		PrimaryCommandList    = gameBase->m_devices[0].CommandList.Get();

		SecondaryDevice       = gameBase->m_devices[1].Device.Get();
		SecondaryCommandQueue = gameBase->m_devices[1].CommandQueue.Get();
		SecondaryCommandList  = gameBase->m_devices[1].CommandList.Get();
#else
		Device                = gameBase->m_devices[0].Device.Get();
		CommandQueue          = gameBase->m_devices[0].CommandQueue.Get();
		CommandList           = gameBase->m_devices[0].CommandList.Get();

		ComputeDevice         = gameBase->m_devices[1].Device.Get();
		ComputeCommandQueue   = gameBase->m_devices[1].CommandQueue.Get();
		ComputeCommandList    = gameBase->m_devices[1].CommandList.Get();

		Command.gameBase      = gameBase;
		Command.queue         = gameBase->m_devices[0].CommandQueue.Get();
		Command.list          = gameBase->m_devices[0].CommandList.Get();
#endif
		DescriptorHeaps.resize(1);
#if DXTK_D3D11 || DXTK_DWRITE
		Device11              = gameBase->m_d3d11Device.Get();
		Direct2D              = gameBase->m_d2dDeviceContext.Get();
		DirectWrite           = gameBase->m_dWriteFactory.Get();
#endif
#if DXTK_D3D9
		Device9               = gameBase->m_d3d9Device.Get();
		Direct3D9             =&m_d3d9;
		Direct3D9->Attach(m_gameBase);
		DX9::SpriteBatch      = std::make_unique<DX9::SpriteBatch9>(Device9);
#endif
		AudioEngine           = gameBase->m_audioEngine.get();
		Audio.engine          = AudioEngine;
		Audio.retry           =&gameBase->m_retryAudio;

//#if DXTK_AMP
//		PrimaryAMP   = std::make_unique<concurrency::accelerator_view>(m_gameBase->m_primaryAccelerator  ->get_default_view());
//		SecondaryAMP = std::make_unique<concurrency::accelerator_view>(m_gameBase->m_secondaryAccelerator->get_default_view());
//#endif
	}

	GameBase*                m_gameBase = nullptr;

#if DXTK_D3D9
	direct3d9                m_d3d9;
#endif
};

extern dxtk* DXTK;
