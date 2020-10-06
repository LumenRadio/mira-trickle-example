# Trickle Example Application

This application demonstrates how one can use Trickle on top of Mira's link-local multicast feature to accomplish a network wide broadcast functionallity, here called Distribution Service.

Mira version 2.4.0 or later is required.

## Application

This example implements two nodes: a root application and a mesh-node application.
Each application make use of the Distribution service in order to propagate data.

### Root

The root application initiates the Mira network and the distribution of a `global_state` variable. It does an update to the global_state at a defined interval and sends it to the defined link-local multicast address.

### Mesh-Node

The mesh-node application joins the network and sends a "Hello Network" packet to the root node every minute using unicast UDP packet.
The applications also registers a distribution service to be able to recieve the packets with the same ID.

After the node is joined the node will receive the updated global_state. With the Distribution service it will also automatically re-distribute the new data.

Multiple Mesh-nodes with this application can run in the same network without changing the code, the distribution will work regardless.

### Building and flashing

To build root and node application you need the Mira library.
```
cd root
make LIBDIR=<path_to_mira_lib> flash.<programmer_serial>
cd ../node
make LIBDIR=<path_to_mira_lib> flash.<programmer_serial>
```

or change the path to the Mira library in the Makefiles.

## Distribution Service

The Distribution service is built on top of the Trickle Algorithm, read more about it here: https://tools.ietf.org/html/rfc6206

The service is built to handle mulitple indepentend contexts of a Trickle instance, each with its unique ID.

The Trickle algorithm implementation is located in `trickle_timer.c`. The Distribution Service make use of the tickle timer and MiraMesh in order to propagate the data. The only functions that needs to be called from the application can be found in `distribution_service.h`

The packet structure sent over the Distrubtion Service:

| 4 bytes | 4 bytes | max 230 bytes |
|---------|---------|---------------|
|    ID   | Version |      Data     |


# Disclaimer

Copyright (c) 2020, LumenRadio AB All rights reserved.

The software is provided as an example of how to use Trickle. The software is delivered without guarantees.

No guarantees is taken for protocol stability of the software, and future updates may change API.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.