

static const uint TileSizeX = 16;
static const uint TileSizeY = 8;
static const uint PixelPerTile = 128;
static const uint SamplesPerPixel = 4;
static const uint InstanceIdMask = 1023;
static const uint InstanceIdBits = 10;
static const uint NumClosures = 22;

cbuffer SystemConstant : register(b0) {
	int2 ImageSize;
}

struct WorkItem {
	uint tile;
	uint closureId;
};

Texture2D<uint2> visibilityBuffer : register(t0);
RWStructuredBuffer<uint4> closureToTileCountMap : register(u0);
AppendStructuredBuffer<WorkItem> jobList : register(u1);

groupshared uint scoreboard[NumClosures];
[numthreads(TileSizeX,TileSizeY,1)]
void main(uint3 localId : SV_GroupThreadID, uint3 globalId : SV_DispatchThreadID) {
	const uint2 MyPixel = globalId.xy;
	const uint TileIndex = ((MyPixel.y / TileSizeY) << 16) | (MyPixel.x / TileSizeX);
	const uint LocalThreadId = localId.x + localId.y * TileSizeX;
	
	if (LocalThreadId == 0) {
		[unroll]
		for (int i = 0; i < NumClosures; i++)
			scoreboard[i] = 0;
	}
	
	GroupMemoryBarrierWithGroupSync();
	
	uint r = visibilityBuffer.Load(uint3(MyPixel, 0));
	[unroll]
	for (uint a = 0; a < SamplesPerPixel; a++) {
		uint instanceId = visibilityBuffer.Load(uint3(MyPixel, a)).y;
		scoreboard[instanceId] = 1;
	}
	
	GroupMemoryBarrierWithGroupSync();
	uint c = LocalThreadId;
	while (c < NumClosures) {
		[branch]
		if (scoreboard[c] > 0) {
			uint x;
			InterlockedAdd(closureToTileCountMap[c].x, 1, x);
			WorkItem item;
			item.tile = TileIndex;
			item.closureId = c;
			jobList.Append(item);
		}
		c += PixelPerTile;
	}
}