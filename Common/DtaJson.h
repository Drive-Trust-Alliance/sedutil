/* C:B**************************************************************************
   This software is Copyright (c) 2014-2024 Bright Plaza Inc. <drivetrust@drivetrust.com>

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
#if !defined(__DTAJSON_H_INCLUDED__)
#define __DTAJSON_H_INCLUDED__

#undef min
#undef max

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wconversion"
#endif // defined(__clang__)
#include "json_struct.h"
#if defined(__clang__)
#pragma clang diagnostic pop
#endif // defined(__clang__)

#include "DtaStructures.h"

namespace JS {
    template<>
    struct TypeHandler<DTA_DEVICE_TYPE>
    {
    public:
        static inline Error to(DTA_DEVICE_TYPE& to_type, ParseContext& context)
        {
            char* pointer;
            unsigned long value = strtoul(context.token.value.data, &pointer, 10);
            to_type = static_cast<DTA_DEVICE_TYPE>((unsigned int)value);
            if (context.token.value.data == pointer)
                return Error::FailedToParseInt;
            return Error::NoError;
        }

        static void from(const DTA_DEVICE_TYPE& from_type, Token& token, Serializer& serializer)
        {
            std::string buf = std::to_string((const unsigned int)from_type);
            token.value_type = Type::Number;
            token.value.data = buf.data();
            token.value.size = buf.size();
            serializer.write(token);
        }
    };
}
JS_OBJ_EXT(DTA_DEVICE_INFO,
           Unknown,
           VendorSpecific,
           TPer,
           Locking,
           Geometry,
           Enterprise,
           SingleUser,
           DataStore,
           OPAL20,
           OPAL10,
           Properties,
           ANY_OPAL_SSC,
           OPALITE,
           PYRITE,
           PYRITE2,
           RUBY,
           BlockSID,
           DataRemoval,
           NSLocking,
           FIPS,
           TPer_ACKNACK,
           TPer_async,
           TPer_bufferMgt,
           TPer_comIDMgt,
           TPer_streaming,
           TPer_sync,
           Locking_locked,
           Locking_lockingEnabled,
           Locking_lockingSupported,
           Locking_MBRshadowingNotSupported,
           Locking_MBRDone,
           Locking_MBREnabled,
           Locking_mediaEncrypt,
           Geometry_align,
           Geometry_alignmentGranularity,
           Geometry_logicalBlockSize,
           Geometry_lowestAlignedLBA,
           Enterprise_rangeCrossing,
           Enterprise_basecomID,
           Enterprise_numcomID,
           SingleUser_any,
           SingleUser_all,
           SingleUser_policy,
           SingleUser_lockingObjects,
           DataStore_maxTables,
           DataStore_maxTableSize,
           DataStore_alignment,
           OPAL10_basecomID,
           OPAL10_numcomIDs,
           OPAL20_basecomID,
           OPAL20_numcomIDs,
           OPAL20_initialPIN,
           OPAL20_revertedPIN,
           OPAL20_version,
           OPAL20_minor_v,
           OPAL20_numAdmins,
           OPAL20_numUsers,
           OPAL20_rangeCrossing,
           OPALITE_version,
           OPALITE_basecomID,
           OPALITE_numcomIDs,
           OPALITE_initialPIN,
           OPALITE_revertedPIN,
           PYRITE_version,
           PYRITE_basecomID,
           PYRITE_numcomIDs,
           PYRITE_initialPIN,
           PYRITE_revertedPIN,
           PYRITE2_version,
           PYRITE2_basecomID,
           PYRITE2_numcomIDs,
           PYRITE2_initialPIN,
           PYRITE2_revertedPIN,
           RUBY_version,
           RUBY_basecomID,
           RUBY_numcomIDs,
           RUBY_numAdmins,
           RUBY_numUsers,
           RUBY_initialPIN,
           RUBY_revertedPIN,
           BlockSID_BlockSIDState,
           BlockSID_SIDvalueState,
           BlockSID_HardReset,
           DataRemoval_version,
           DataRemoval_OperationProcessing,
           DataRemoval_Mechanism,
           DataRemoval_TimeFormat_Bit5,
           DataRemoval_Time_Bit5,
           DataRemoval_TimeFormat_Bit4,
           DataRemoval_Time_Bit4,
           DataRemoval_TimeFormat_Bit3,
           DataRemoval_Time_Bit3,
           DataRemoval_TimeFormat_Bit2,
           DataRemoval_Time_Bit2,
           DataRemoval_TimeFormat_Bit1,
           DataRemoval_Time_Bit1,
           DataRemoval_TimeFormat_Bit0,
           DataRemoval_Time_Bit0,
           NSLocking_version,
           range_C,
           range_P,
           Max_Key_Count,
           Unused_Key_Count,
           Max_Range_Per_NS,
           devSize,
           devType,
           serialNum,
           firmwareRev,
           modelNum,
           vendorID,
           manufacturerName,
           worldWideName,
           physicalInterconnect,
           physicalInterconnectLocation,
           passwordSalt,
           fips,
           asmedia,
           enclosure);
	
#endif // !defined(__DTAJSON_H_INCLUDED__)
