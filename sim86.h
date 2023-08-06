#pragma once

#define BYTES_PER_SECTOR 512

typedef struct{
	int cylinders;
	int tracks_per_cylinder; //# of heads/sides
	int sectors_per_track; //sectors per track/head
} HDD_DEVICE;