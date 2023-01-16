


static const uint NumWorkerThreads = 256;
static const uint NumClosures = 22;

static const uint NumWorkItem = 256;


struct WorkItem {
	uint tile;
	uint closureId;
};


StructuredBuffer<uint4> closureToTileCountMap : register(t0);
ConsumeStructuredBuffer<WorkItem> jobList : register(u0);
RWStructuredBuffer<uint4> offsets : register(u1);
RWStructuredBuffer<uint4> tiles : register(u2);

[numthreads(NumWorkerThreads, 1, 1)]
void main(uint3 globalThreadId : SV_DispatchThreadID) {
	
	if (globalThreadId.x == 0) {
		offsets[0].x = offsets[0].x = 0;
		for (int i = 2; i < NumClosures + 1; i++)
			offsets[i] = offsets[i - 1] + closureToTileCountMap[i - 2].x; 
	}
	
	DeviceMemoryBarrierWithGroupSync();
	
	uint count = globalThreadId.x;
	while (count < NumWorkItem) {
		WorkItem item = jobList.Consume();
		
		uint idx;
		InterlockedAdd(offsets[item.closureId + 1].x, 1, idx);
		tiles[idx] = item.tile;
		
		count += NumWorkItem;
	}
}