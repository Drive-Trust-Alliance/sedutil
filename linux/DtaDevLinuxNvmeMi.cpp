/* C:B**************************************************************************
This software is Copyright 2014-2017 Bright Plaza Inc. <drivetrust@drivetrust.com>

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
#include "config.h"
#include "os.h"
#include "DtaDevLinuxNvmeMi.h"

#include <cstring>

constexpr int tcgDefaultTimeoutMS = 20*1000;

static int parse_mi_dev(const char *dev, unsigned int *net, uint8_t *eid,
                        unsigned int *ctrl)
{
    int rc;

    /* <net>,<eid>:<ctrl-id> form */
    rc = sscanf(dev, "mctp:%u,%hhu:%u", net, eid, ctrl);
    if (rc == 3)
        return 0;

    /* <net>,<eid> form, implicit ctrl-id = 0 */
    *ctrl = 0;
    rc = sscanf(dev, "mctp:%u,%hhu", net, eid);
    if (rc == 2)
        return 0;

    return -1;
}


/** The Device class represents a single disk device.
 *  Linux specific implementation using the NVMe interface
 */
DtaDevLinuxNvmeMi::DtaDevLinuxNvmeMi() {
    net = 0;
    eid = 0;
    ctrl_id = 0;

    root = NULL;
    endpoint = NULL;
    controller = NULL;
}

bool DtaDevLinuxNvmeMi::init(const char * devref)
{
    LOG(D1) << "Creating DtaDevLinuxNvmeMi::init " << devref;

    int rc =  parse_mi_dev(devref, &net, &eid, &ctrl_id);
    if (rc)
    {
        LOG(E) << "invalid nvme-mi device specifier: " << devref;
        goto error_free;
    }

    root = nvme_mi_create_root(stderr, LOG_WARNING);
    if (!root)
    {
        LOG(E) << "can't create NVMe root";
        goto error_free;
    }

    endpoint = nvme_mi_open_mctp(root, net, eid);
    if (!endpoint)
    {
        LOG(E) << "can't open MCTP endpoint " << devref;
        goto error_free;
    }

    controller = nvme_mi_init_ctrl(endpoint, ctrl_id);
    if (!controller)
    {
        LOG(E) << "can't open MCTP controller " << devref;
        goto error_free;
    }

    return TRUE; // isOpen = true

error_free:
    if (controller) {
        nvme_mi_close_ctrl(controller);
        controller = NULL;
    }

    if (endpoint) {
        nvme_mi_close(endpoint);
        endpoint = NULL;
    }

    if (root) {
        nvme_mi_free_root(root);
        root = NULL;
    }
    return FALSE; // isOpen = false
}

/** Send an ioctl to the device using nvme admin commands. */
uint8_t DtaDevLinuxNvmeMi::sendCmd(ATACOMMAND cmd, uint8_t protocol,
        uint16_t comID, void * buffer, uint32_t bufferlen)
{
    int rc;
    LOG(D1) << "Entering DtaDevLinuxNvmeMi::sendCmd()";
    if (IF_RECV == cmd) {
        LOG(D3) << "Security Receive Command";
        struct nvme_security_receive_args args = { 0 };
        args.args_size = sizeof(args);
        args.secp = protocol;
        args.spsp0 = comID & 0xFF;
        args.spsp1 = (comID >> 8);
        args.al = bufferlen;
        args.data_len = bufferlen;
        args.data = buffer;

        unsigned int timeout = nvme_mi_ep_get_timeout(endpoint);
        nvme_mi_ep_set_timeout(endpoint, tcgDefaultTimeoutMS);
        rc = nvme_mi_admin_security_recv(controller, &args);
        nvme_mi_ep_set_timeout(endpoint, timeout);
        if (rc < 0)
        {
            // transport layer error
            LOG(E) << "security-receive failed: " << std::strerror(errno);
            return rc;
        }
    }
    else {
        LOG(D3) << "Security Send Command";
        nvme_security_send_args args= { 0 };
        args.args_size = sizeof(args);
        args.secp = protocol;
        args.spsp0 = comID & 0xFF;
        args.spsp1 = (comID >> 8);
        args.tl = bufferlen;
        args.data_len = bufferlen;
        args.data = buffer;

        unsigned int timeout = nvme_mi_ep_get_timeout(endpoint);
        nvme_mi_ep_set_timeout(endpoint, tcgDefaultTimeoutMS);
        rc = nvme_mi_admin_security_send(controller, &args);
        nvme_mi_ep_set_timeout(endpoint, timeout);
        if (rc < 0) {
            // transport layer error
            LOG(E) << "security-send failed: " << std::strerror(errno);
            return rc;
        }
    }

    if (rc != 0) {
        // Status Field in Complection Queue Entry is not zero
        LOG(E) << "NVME Security Command Error:" <<  std::hex << rc << std::dec;
    }
    return rc;
}

void DtaDevLinuxNvmeMi::identify(OPAL_DiskInfo& disk_info)
{
    LOG(D4) << "Entering DtaDevLinuxNvmeMi::identify()";
    int rc = nvme_mi_admin_identify_ctrl(controller, &id_ctrl);
    if (rc) {
        LOG(E) << "Identify Controller failed: " << std::strerror(errno);
        disk_info.devType = DEVICE_TYPE_OTHER;
        return;
    }

    disk_info.devType = DEVICE_TYPE_NVME;
    memcpy(disk_info.serialNum, id_ctrl.sn, sizeof(disk_info.serialNum));
    memcpy(disk_info.modelNum, id_ctrl.mn, sizeof(disk_info.modelNum));
    memcpy(disk_info.firmwareRev, id_ctrl.fr, sizeof(disk_info.firmwareRev));
    return;
}

/** Close the device reference so this object can be delete. */
DtaDevLinuxNvmeMi::~DtaDevLinuxNvmeMi()
{
    LOG(D1) << "Destroying DtaDevLinuxNvmeMi";
    if (controller) {
        nvme_mi_close_ctrl(controller);
    }

    if (endpoint) {
        nvme_mi_close(endpoint);
    }

    if (root) {
        nvme_mi_free_root(root);
    }
}
