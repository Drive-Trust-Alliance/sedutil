/* C:B**************************************************************************
This software is © 2014 Bright Plaza Inc. <drivetrust@drivetrust.com>

This file is part of sedutil.

sedutil is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

sedutil is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with sedutil.  If not, see <http://www.gnu.org/licenses/>.

 * C:E********************************************************************** */
#pragma once
#if !defined(__NVMESTRUCTURES_H_INCLUDED__)
#define __NVMESTRUCTURES_H_INCLUDED__

/** NVMe commands needed for TCG storage communication */


/*
 * From the NVM Express Base Specification, Revision 2.1
 * © Copyright 2008 to 2024 NVM Express, Inc. ALL RIGHTS RESERVED.
 *
 * See Figure 141
 *
 */
typedef enum _NVMe_COMMAND {
  NVME_IDENTIFY      = 0x06 ,
      NVME_IDENTIFY_NAMESPACE_DATA_STRUCTURE  = 0x00,
      NVME_IDENTIFY_CONTROLLER_DATA_STRUCTURE = 0x01,
  NVME_SECURITY_SEND = 0x81 ,
  NVME_SECURITY_RECV = 0x82 ,
} NVMe_COMMAND;




/**
 * From /usr/include/nvme/types.h
 *
 */

// SPDX-License-Identifier: LGPL-2.1-or-later
/*
 * This file is part of libnvme.
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors: Keith Busch <keith.busch@wdc.com>
 *	    Chaitanya Kulkarni <chaitanya.kulkarni@wdc.com>
 */

/**
 * enum nvme_constants - A place to stash various constant nvme values
 * @NVME_NSID_ALL:		A broadcast value that is used to specify all
 *				namespaces
 * @NVME_NSID_NONE:		The invalid namespace id, for when the nsid
 *				parameter is not used in a command
 * @NVME_UUID_NONE:		Use to omit a uuid command parameter
 * @NVME_CNTLID_NONE:		Use to omit a cntlid command parameter
 * @NVME_CNSSPECID_NONE:	Use to omit a cns_specific_id command parameter
 * @NVME_LOG_LSP_NONE:		Use to omit a log lsp command parameter
 * @NVME_LOG_LSI_NONE:		Use to omit a log lsi command parameter
 * @NVME_LOG_LPO_NONE:		Use to omit a log lpo command parameter
 * @NVME_IDENTIFY_DATA_SIZE:	The transfer size for nvme identify commands
 * @NVME_LOG_SUPPORTED_LOG_PAGES_MAX: The largest possible index in the supported
 *				log pages log.
 * @NVME_ID_NVMSET_LIST_MAX:	The largest possible nvmset index in identify
 *				nvmeset
 * @NVME_ID_UUID_LIST_MAX:	The largest possible uuid index in identify
 *				uuid list
 * @NVME_ID_CTRL_LIST_MAX:	The largest possible controller index in
 *				identify controller list
 * @NVME_ID_NS_LIST_MAX:	The largest possible namespace index in
 *				identify namespace list
 * @NVME_ID_SECONDARY_CTRL_MAX:	The largest possible secondary controller index
 *				in identify secondary controller
 * @NVME_ID_DOMAIN_LIST_MAX:	The largest possible domain index in the
 *				in domain list
 * @NVME_ID_ENDURANCE_GROUP_LIST_MAX: The largest possible endurance group
 *				index in the endurance group list
 * @NVME_ID_ND_DESCRIPTOR_MAX:	The largest possible namespace granularity
 *				index in the namespace granularity descriptor
 *				list
 * @NVME_FEAT_LBA_RANGE_MAX:	The largest possible LBA range index in feature
 *				lba range type
 * @NVME_LOG_ST_MAX_RESULTS:	The largest possible self test result index in the
 *				device self test log
 * @NVME_LOG_FID_SUPPORTED_EFFECTS_MAX:	The largest possible FID index in the
 *				feature	identifiers effects log.
 * @NVME_LOG_MI_CMD_SUPPORTED_EFFECTS_MAX:	The largest possible MI Command index
 *				in the MI Command effects log.
 * @NVME_LOG_MI_CMD_SUPPORTED_EFFECTS_RESERVED:	The reserved space in the MI Command
 *				effects log.
 * @NVME_LOG_TELEM_BLOCK_SIZE:	Specification defined size of Telemetry Data Blocks
 * @NVME_DSM_MAX_RANGES:	The largest possible range index in a data-set
 *				management command
 * @NVME_NQN_LENGTH:		Max length for NVMe Qualified Name
 * @NVMF_TRADDR_SIZE:		Max Transport Address size
 * @NVMF_TSAS_SIZE:		Max Transport Specific Address Subtype size
 * @NVME_ZNS_CHANGED_ZONES_MAX: Max number of zones in the changed zones log
 *				page
 */
enum nvme_constants {
	NVME_NSID_ALL				= 0xffffffff,
	NVME_NSID_NONE				= 0,
	NVME_UUID_NONE				= 0,
	NVME_CNTLID_NONE			= 0,
	NVME_CNSSPECID_NONE			= 0,
	NVME_LOG_LSP_NONE			= 0,
	NVME_LOG_LSI_NONE			= 0,
	NVME_LOG_LPO_NONE			= 0,
	NVME_IDENTIFY_DATA_SIZE			= 4096,
	NVME_LOG_SUPPORTED_LOG_PAGES_MAX	= 256,
	NVME_ID_NVMSET_LIST_MAX			= 31,
	NVME_ID_UUID_LIST_MAX			= 127,
	NVME_ID_CTRL_LIST_MAX			= 2047,
	NVME_ID_NS_LIST_MAX			= 1024,
	NVME_ID_SECONDARY_CTRL_MAX		= 127,
	NVME_ID_DOMAIN_LIST_MAX			= 31,
	NVME_ID_ENDURANCE_GROUP_LIST_MAX	= 2047,
	NVME_ID_ND_DESCRIPTOR_MAX		= 16,
	NVME_FEAT_LBA_RANGE_MAX			= 64,
	NVME_LOG_ST_MAX_RESULTS			= 20,
	NVME_LOG_TELEM_BLOCK_SIZE		= 512,
	NVME_LOG_FID_SUPPORTED_EFFECTS_MAX	= 256,
	NVME_LOG_MI_CMD_SUPPORTED_EFFECTS_MAX	= 256,
	NVME_LOG_MI_CMD_SUPPORTED_EFFECTS_RESERVED = 768,
	NVME_DSM_MAX_RANGES			= 256,
	NVME_NQN_LENGTH				= 256,
	NVMF_TRADDR_SIZE			= 256,
	NVMF_TSAS_SIZE				= 256,
	NVME_ZNS_CHANGED_ZONES_MAX		= 511,
};

/**
 * enum nvme_identify_cns -			Identify - CNS Values
 * @NVME_IDENTIFY_CNS_NS:			Identify Namespace data structure
 * @NVME_IDENTIFY_CNS_CTRL:			Identify Controller data structure
 * @NVME_IDENTIFY_CNS_NS_ACTIVE_LIST:		Active Namespace ID list
 * @NVME_IDENTIFY_CNS_NS_DESC_LIST:		Namespace Identification Descriptor list
 * @NVME_IDENTIFY_CNS_NVMSET_LIST:		NVM Set List
 * @NVME_IDENTIFY_CNS_CSI_NS:			I/O Command Set specific Identify
 *						Namespace data structure
 * @NVME_IDENTIFY_CNS_CSI_CTRL:			I/O Command Set specific Identify
 *						Controller data structure
 * @NVME_IDENTIFY_CNS_CSI_NS_ACTIVE_LIST:	Active Namespace ID list associated
 *						with the specified I/O Command Set
 * @NVME_IDENTIFY_CNS_CSI_INDEPENDENT_ID_NS:	I/O Command Set Independent Identify
 * @NVME_IDENTIFY_CNS_NS_USER_DATA_FORMAT:	Namespace user data format
 * @NVME_IDENTIFY_CNS_CSI_NS_USER_DATA_FORMAT:	I/O Command Set specific user data
 *						format
 *						Namespace data structure
 * @NVME_IDENTIFY_CNS_ALLOCATED_NS_LIST:	Allocated Namespace ID list
 * @NVME_IDENTIFY_CNS_ALLOCATED_NS:		Identify Namespace data structure for
 *						the specified allocated NSID
 * @NVME_IDENTIFY_CNS_NS_CTRL_LIST:		Controller List of controllers attached
 *						to the specified NSID
 * @NVME_IDENTIFY_CNS_CTRL_LIST:		Controller List of controllers that exist
 *						in the NVM subsystem
 * @NVME_IDENTIFY_CNS_PRIMARY_CTRL_CAP:		Primary Controller Capabilities data
 *						structure for the specified primary controller
 * @NVME_IDENTIFY_CNS_SECONDARY_CTRL_LIST:	Secondary Controller list of controllers
 *						associated with the primary controller
 *						processing the command
 * @NVME_IDENTIFY_CNS_NS_GRANULARITY:		A Namespace Granularity List
 * @NVME_IDENTIFY_CNS_UUID_LIST:		A UUID List
 * @NVME_IDENTIFY_CNS_DOMAIN_LIST:		Domain List
 * @NVME_IDENTIFY_CNS_ENDURANCE_GROUP_ID:	Endurance Group List
 * @NVME_IDENTIFY_CNS_CSI_ALLOCATED_NS_LIST:	I/O Command Set specific Allocated Namespace
 *						ID list
 * @NVME_IDENTIFY_CNS_CSI_ID_NS_DATA_STRUCTURE:	I/O Command Set specific ID Namespace
 *						Data Structure for Allocated Namespace ID
 * @NVME_IDENTIFY_CNS_COMMAND_SET_STRUCTURE:	Base Specification 2.0a section 5.17.2.21
 * @NVME_IDENTIFY_CNS_SUPPORTED_CTRL_STATE_FORMATS:	Supported Controller State Formats
 *							identifying the supported NVMe Controller
 *							State data structures
 */
enum nvme_identify_cns {
	NVME_IDENTIFY_CNS_NS					= 0x00,
	NVME_IDENTIFY_CNS_CTRL					= 0x01,
	NVME_IDENTIFY_CNS_NS_ACTIVE_LIST			= 0x02,
	NVME_IDENTIFY_CNS_NS_DESC_LIST				= 0x03,
	NVME_IDENTIFY_CNS_NVMSET_LIST				= 0x04,
	NVME_IDENTIFY_CNS_CSI_NS				= 0x05,
	NVME_IDENTIFY_CNS_CSI_CTRL				= 0x06,
	NVME_IDENTIFY_CNS_CSI_NS_ACTIVE_LIST			= 0x07,
	NVME_IDENTIFY_CNS_CSI_INDEPENDENT_ID_NS			= 0x08,
	NVME_IDENTIFY_CNS_NS_USER_DATA_FORMAT			= 0x09,
	NVME_IDENTIFY_CNS_CSI_NS_USER_DATA_FORMAT		= 0x0A,
	NVME_IDENTIFY_CNS_ALLOCATED_NS_LIST			= 0x10,
	NVME_IDENTIFY_CNS_ALLOCATED_NS				= 0x11,
	NVME_IDENTIFY_CNS_NS_CTRL_LIST				= 0x12,
	NVME_IDENTIFY_CNS_CTRL_LIST				= 0x13,
	NVME_IDENTIFY_CNS_PRIMARY_CTRL_CAP			= 0x14,
	NVME_IDENTIFY_CNS_SECONDARY_CTRL_LIST			= 0x15,
	NVME_IDENTIFY_CNS_NS_GRANULARITY			= 0x16,
	NVME_IDENTIFY_CNS_UUID_LIST				= 0x17,
	NVME_IDENTIFY_CNS_DOMAIN_LIST				= 0x18,
	NVME_IDENTIFY_CNS_ENDURANCE_GROUP_ID			= 0x19,
	NVME_IDENTIFY_CNS_CSI_ALLOCATED_NS_LIST			= 0x1A,
	NVME_IDENTIFY_CNS_CSI_ID_NS_DATA_STRUCTURE		= 0x1B,
	NVME_IDENTIFY_CNS_COMMAND_SET_STRUCTURE			= 0x1C,
	NVME_IDENTIFY_CNS_SUPPORTED_CTRL_STATE_FORMATS		= 0x20,
};

/**
 * From /usr/include/linux/asm-generic/int-ll64.h
 *
 */
/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
/*
 * asm-generic/int-ll64.h
 *
 * Integer declarations for architectures which use "long long"
 * for 64-bit types.
 */
#if defined(_MSC_VER)
#define __signed__ signed
#endif
typedef __signed__ char __s8;
typedef unsigned char __u8;

typedef __signed__ short __s16;
typedef unsigned short __u16;

typedef __signed__ int __s32;
typedef unsigned int __u32;

#ifdef __GNUC__
__extension__ typedef __signed__ long long __s64;
__extension__ typedef unsigned long long __u64;
#else
typedef __signed__ long long __s64;
typedef unsigned long long __u64;
#endif


/**
 * From /usr/include/linux/nvme_ioctl.h
 *
 * SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note
 *
 * Definitions for the NVM Express ioctl interface
 * Copyright (c) 2011-2014, Intel Corporation.
 */
#pragma pack(1)
struct nvme_user_io {
	__u8	opcode;
	__u8	flags;
	__u16	control;
	__u16	nblocks;
	__u16	rsvd;
	__u64	metadata;
	__u64	addr;
	__u64	slba;
	__u32	dsmgmt;
	__u32	reftag;
	__u16	apptag;
	__u16	appmask;
};

struct nvme_passthru_cmd {
	__u8	opcode;
	__u8	flags;
	__u16	rsvd1;
	__u32	nsid;
	__u32	cdw2;
	__u32	cdw3;
	__u64	metadata;
	__u64	addr;
	__u32	metadata_len;
	__u32	data_len;
	__u32	cdw10;
	__u32	cdw11;
	__u32	cdw12;
	__u32	cdw13;
	__u32	cdw14;
	__u32	cdw15;
	__u32	timeout_ms;
	__u32	result;
};

struct nvme_passthru_cmd64 {
	__u8	opcode;
	__u8	flags;
	__u16	rsvd1;
	__u32	nsid;
	__u32	cdw2;
	__u32	cdw3;
	__u64	metadata;
	__u64	addr;
	__u32	metadata_len;
	union {
		__u32	data_len; /* for non-vectored io */
		__u32	vec_cnt; /* for vectored io */
	};
	__u32	cdw10;
	__u32	cdw11;
	__u32	cdw12;
	__u32	cdw13;
	__u32	cdw14;
	__u32	cdw15;
	__u32	timeout_ms;
	__u32   rsvd2;
	__u64	result;
};

/* same as struct nvme_passthru_cmd64, minus the 8b result field */
struct nvme_uring_cmd {
	__u8	opcode;
	__u8	flags;
	__u16	rsvd1;
	__u32	nsid;
	__u32	cdw2;
	__u32	cdw3;
	__u64	metadata;
	__u64	addr;
	__u32	metadata_len;
	__u32	data_len;
	__u32	cdw10;
	__u32	cdw11;
	__u32	cdw12;
	__u32	cdw13;
	__u32	cdw14;
	__u32	cdw15;
	__u32	timeout_ms;
	__u32   rsvd2;
};

#define nvme_admin_cmd nvme_passthru_cmd
#pragma pack()

#endif //!defined(__NVMESTRUCTURES_H_INCLUDED__)
