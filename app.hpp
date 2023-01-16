
#include <Windows.h>

#include "framework/device.h"
#include "framework/commandbuffer.h"
#include "framework/descriptor_heap.h"
#include "framework/pipeline.h"
#include "framework/queue.h"
#include "framework/root_signature.h"
#include "framework/shader.h"
#include "framework/buffer.h"
#include "framework/texture.h"
#include "framework/fence.h"

#include "tools/my_gui.h"


#include "tools/model.h"

#include "glm-master/glm/glm.hpp"
#include "glm-master/glm/gtc/matrix_transform.hpp"
#include "glm-master/glm/gtc/quaternion.hpp"

class App {
public:
	App();
	~App();

	bool initialize(HWND hwnd);

	void shutdown();

	void render();
	
	void run(UINT curImageCount);

private:
	Device m_device;
	Queue m_queue;
	Swapchain m_swapchain;

	std::vector<CommandAllocator> m_commandAllocator;
	std::vector<CommandList> m_commandList;

	RootSignature m_rootSignature;
	RootSignature m_materialCountRS;
	RootSignature m_materialSortRS;
	RootSignature m_renderingRS;

	Pipeline m_pipeline;
	
	ComputePipeline m_materialCountPipeline;
	ComputePipeline m_materialSortPipeline;
	ComputePipeline m_renderingPipeline;

	Fence m_presentFence;

	Model m_model;

	int m_backBuffer;
	int m_depthBuffer;
	int m_visibilityBuffer;
	int m_renderingBuffer;
	int m_cb0;
	int m_systemCB;
	int m_wrapSampler;

	int m_vs;
	int m_ps;

	int m_materialCountCS;
	int m_materialSortCS;
	int m_renderingCS;

	MyGui m_gui;
};