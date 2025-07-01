#ifndef ADVERTISING_H
#define ADVERTISING_H

// BLE Advertising Types (as defined in Bluetooth Core Spec Vol 6, Part B, Section 4.3.2)

#define ADV_TYPE_IND                        0x00u  // Connectable undirected advertising (default)
#define ADV_TYPE_DIRECT_IND_HIGH            0x01u  // Connectable high duty cycle directed advertising
#define ADV_TYPE_SCAN_IND                   0x02u  // Scannable undirected advertising
#define ADV_TYPE_NONCONN_IND                0x03u  // Non-connectable undirected advertising
#define ADV_TYPE_DIRECT_IND_LOW             0x04u  // Connectable low duty cycle directed advertising

#endif // ADVERTISING_H
