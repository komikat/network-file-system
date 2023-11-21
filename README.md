We are assuming that storage server is not managed manually by the user when it is running.

ERROR CODES:

Assumption: Storage server needs to be initialized with a root directory where all the data for that server will be
stored. It will not be created automatically to avoid excessive control over the file system.

- 001: No root directory found on a storage server
- 002: Multiple root directories found
- 003: Unable to send data over the network
- 004: Issues with polling for clients
- 005: Error receiving request, asking to resend
- 006: Invalid Request
- 008: Memory exceeded