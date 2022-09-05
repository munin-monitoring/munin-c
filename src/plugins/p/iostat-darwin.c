/*
 * Copyright (C) 2018 Michal Sojka <wsh@2x.cz> - All rights reserved.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU General Public License v.2 or v.3.
 */

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/storage/IOBlockStorageDriver.h>
#include <IOKit/storage/IOMedia.h>
#include <IOKit/IOBSD.h>
#include "common.h"
#include "plugins.h"

typedef void (*deviceCallback)(char *name, uint64_t reads, uint64_t writes);
static void enumerateDevices(deviceCallback cb);
static void enumerateDevice(void *context, io_iterator_t drivelist);
static void configCallback(char *name, uint64_t reads, uint64_t writes);
static void fetchCallback(char *name, uint64_t reads, uint64_t writes);

int iostat(int argc, char **argv)
{
	if (argc > 1) {
		if (!strcmp(argv[1], "config")) {
			puts("graph_title IOstat\n"
			     "graph_args --base 1024\n"
			     "graph_vlabel blocks per ${graph_period} read (-) / written (+)\n"
			     "graph_category disk\n"
				 "graph_info This graph shows the I/O to and from block devices.");

			enumerateDevices(configCallback);
			return 0;
		}
		if (!strcmp(argv[1], "autoconf"))
			return writeyes();
	}

	enumerateDevices(fetchCallback);
	return 0;
}

static void enumerateDevices(deviceCallback cb)
{
	io_iterator_t drivelist;
	CFMutableDictionaryRef match = IOServiceMatching("IOMedia");
	CFDictionaryAddValue(match, CFSTR(kIOMediaWholeKey), kCFBooleanTrue);
	IONotificationPortRef notifyPort = IONotificationPortCreate(kIOMainPortDefault);
	if (IOServiceAddMatchingNotification(notifyPort, kIOFirstMatchNotification, match, &enumerateDevice, (void *)cb, &drivelist) != KERN_SUCCESS)
		return;

	enumerateDevice((void *)cb, drivelist);
}

static void enumerateDevice(void *context, io_iterator_t drivelist)
{
	deviceCallback cb = (deviceCallback)context;
	io_registry_entry_t drive;
	while ((drive = IOIteratorNext(drivelist))) {
		io_registry_entry_t parent;
		if (IORegistryEntryGetParentEntry(drive, kIOServicePlane, &parent) != KERN_SUCCESS)
			continue;

		if (!IOObjectConformsTo(parent, "IOBlockStorageDriver"))
			continue;

		CFDictionaryRef properties;
		if (IORegistryEntryCreateCFProperties(drive, (CFMutableDictionaryRef *)&properties, kCFAllocatorDefault, kNilOptions) != KERN_SUCCESS)
			continue;

		CFStringRef name = (CFStringRef)CFDictionaryGetValue(properties, CFSTR(kIOBSDNameKey));
		if (!name)
			continue;

		CFIndex length = CFStringGetLength(name);
		CFIndex maxSize = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8);
		char *nameZ = (char *)malloc(maxSize);
		CFStringGetCString(name, nameZ, maxSize, kCFStringEncodingUTF8);

		CFDictionaryRef parentProperties;
		if (IORegistryEntryCreateCFProperties(parent, (CFMutableDictionaryRef *)&parentProperties, kCFAllocatorDefault, kNilOptions) != KERN_SUCCESS)
			continue;

		CFDictionaryRef statistics = (CFDictionaryRef)CFDictionaryGetValue(parentProperties, CFSTR(kIOBlockStorageDriverStatisticsKey));
		if (!statistics)
			continue;

		CFNumberRef number;
		uint64_t reads = 0;
		if ((number = (CFNumberRef)CFDictionaryGetValue(statistics, CFSTR(kIOBlockStorageDriverStatisticsReadsKey))))
			CFNumberGetValue(number, kCFNumberSInt64Type, &reads);

		uint64_t writes = 0;
		if ((number = (CFNumberRef)CFDictionaryGetValue(statistics, CFSTR(kIOBlockStorageDriverStatisticsWritesKey))))
			CFNumberGetValue(number, kCFNumberSInt64Type, &writes);

		cb(nameZ, reads, writes);
	}
}

void configCallback(char *name, __attribute__((unused)) uint64_t reads, __attribute__((unused)) uint64_t writes)
{
	printf("%s_read.label %s\n", name, name);
	printf("%s_read.type DERIVE\n", name);
	printf("%s_read.min 0\n", name);
	printf("%s_read.graph no\n", name);
	printf("%s_write.label %s\n", name, name);
	printf("%s_write.info I/O on device %s\n", name, name);
	printf("%s_write.type DERIVE\n", name);
	printf("%s_write.min 0\n", name);
	printf("%s_write.negative %s_read\n", name, name);
}

void fetchCallback(char *name, uint64_t reads, uint64_t writes)
{
	printf("%s_read.value %lu\n",  name, reads);
	printf("%s_write.value %lu\n", name, writes);
}
