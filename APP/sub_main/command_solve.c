#include "command_solve.h"
uint8_t tmp_command[100];
uint8_t rx_count = 0;

void receive_bytes(uint8_t *d, uint32_t len)
{
    uint16_t i = 0;
    if (tmp_command[0] == 0xaa && tmp_command[1] == 0xbb) {
        memcpy(tmp_command + rx_count, d, min(sizeof(tmp_command) - rx_count, len));
        rx_count += len;
        if (rx_count > sizeof(tmp_command)) {
            memset(tmp_command, 0, sizeof(tmp_command));
            return;
        }
        for (i = 3; i < rx_count; i++) {
            if (tmp_command[i - 1] == 0xcc && tmp_command[i] == 0xdd) {
                command_process(tmp_command, i + 1);
                memset(tmp_command, 0, sizeof(tmp_command));
                return;
            }
        }
    } else {
        for (i = 0; i < len - 1; i++) {
            if (d[i] == 0xaa && d[i + 1] == 0xbb) {
                memcpy(tmp_command, d + i, min(sizeof(tmp_command), len - i));
                rx_count = len - i;
            }
        }
        for (i = 3; i < rx_count; i++) {
            if (tmp_command[i - 1] == 0xcc && tmp_command[i] == 0xdd) {
                command_process(tmp_command, i + 1);
                memset(tmp_command, 0, sizeof(tmp_command));
                return;
            }
        }
    }
}
void command_process(uint8_t *d, uint32_t len)
{
    // ps("receive:");
    // for (int i = 0; i < len; i++)
    //     ps("%02x ", d[i]);
    // ps("\n");
    d += 2;
    len -= 4;
    ps("rec:");
    for (int i = 0; i < len; i++)
        ps("%02x ", d[i]);
    ps("\n");

    ps("len= %d\n", len);
    if (d[0] == 0x01) // change _name
    {
        memset(data_in_fram.device_name, 0, sizeof(data_in_fram.device_name));
        memcpy(data_in_fram.device_name, d + 1, len - 1);
    }
    if (d[0] == 0x02) // change _name
    {
        memset(&data_in_fram.GAP_APPEARE, 0, sizeof(data_in_fram.GAP_APPEARE));
        memcpy(&data_in_fram.GAP_APPEARE, d + 1, len - 1);
    }
    if (d[0] == 0x03) // change _name
    {
        power_reset(0);
        // memset(&data_in_fram.GAP_APPEARE, 0, sizeof(data_in_fram.GAP_APPEARE));
        // memcpy(&data_in_fram.GAP_APPEARE, d + 1, len - 1);
        // ps("GAP_APPEARE = %04x\n", data_in_fram.GAP_APPEARE);
    }

    //! ----------------------------------------------------------------
    if (d[0] == 0x74) {
        if (d[1] == 0x73 || d[1] == 0x74) // combime_key
        {
            memset(data_in_fram.user_key_bind[d[2]], 0, sizeof(data_in_fram.user_key_bind[0]));
            memcpy(data_in_fram.user_key_bind[d[2]] + 2, d + 3, min(len - 3, sizeof(data_in_fram.user_key_bind[0]) - 2));
            data_in_fram.user_key_bind[d[2]][0] = d[1];
            data_in_fram.user_key_bind[d[2]][1] = min(len - 3, sizeof(data_in_fram.user_key_bind[0]) - 2);
            // for (int i = 0; i < 20; i++)
            //     ps("%02x ", data_in_fram.user_key_bind[d[2]][i]);
            // ps("\n");
        }
        if (d[1] == 0x75) {
            if (d[2] <= 3) {
                memset(data_in_fram.user_key_desc[d[2]], 0x20, sizeof(data_in_fram.user_key_desc[0]));
                memcpy(data_in_fram.user_key_desc[d[2]], d + 3, min(len - 3, sizeof(data_in_fram.user_key_desc[0])));
            }
        }
        if (running_data.mode_data == 1) {
            set_mode(running_data.mode_data);
        }
    }
}