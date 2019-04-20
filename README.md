# cfsd-logic-recorder
This microservice provides the recorder handling for Lynx.
The microservice starts and stops the recorder and lables the recorded data correctly based on mission and time of recording.

### Build
AMD64: docker build -f Dockerfile.amd64 -t chalmersfsd/cfsd-logic-recorder:v0.0.0 .