//
//  CDBAccess.hpp
//  SedUserClient
//
//  Created by Jackie Marks on 10/16/15.
//
//

#ifndef CDBAccess_hpp
#define CDBAccess_hpp

extern IODirection getATAMemoryDescriptorDir(SCSICommandDescriptorBlock cdb);
extern uint8_t getATATaskCommandDir(SCSICommandDescriptorBlock cdb);
extern uint64_t getATATransferSize(SCSICommandDescriptorBlock cdb);

#endif /* CDBAccess_hpp */
