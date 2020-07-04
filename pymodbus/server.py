
#!/usr/bin/env python
"""
Pymodbus Asynchronous Server Example
--------------------------------------------------------------------------
The asynchronous server is a high performance implementation using the
twisted library as its backend.  This allows it to scale to many thousands
of nodes which can be helpful for testing monitoring software.
"""
# --------------------------------------------------------------------------- # 
# import the various server implementations
# --------------------------------------------------------------------------- # 
from pymodbus.server.sync import StartSerialServer

from pymodbus.datastore import ModbusSequentialDataBlock
from pymodbus.datastore import ModbusSlaveContext, ModbusServerContext
from pymodbus.transaction import ModbusRtuFramer

# --------------------------------------------------------------------------- # 
# configure the service logging
# --------------------------------------------------------------------------- # 
import logging
FORMAT = ('%(asctime)-15s %(threadName)-15s'
          ' %(levelname)-8s %(module)-15s:%(lineno)-8s %(message)s')
logging.basicConfig(format=FORMAT)
log = logging.getLogger()
log.setLevel(logging.DEBUG)

PORT='/dev/ttyUSB1'

def run_server():

# store value 256 at address 40001; 258 address 40002, 261 address 40003, 263 address 40004, 241 address 40005

    slave_store1 = ModbusSlaveContext(ir=ModbusSequentialDataBlock(40001, [256, 258]), zero_mode=True)
    slave_store2 = ModbusSlaveContext(ir=ModbusSequentialDataBlock(40002, [261, 263]), zero_mode=True)
    slave_store4 = ModbusSlaveContext(ir=ModbusSequentialDataBlock(40004, [269]), zero_mode=True)
    slave_store3 = ModbusSlaveContext(ir=ModbusSequentialDataBlock(40005, [258, 261]), zero_mode=True)
    slaves = {
       0x01: slave_store1,
       0x02: slave_store2,
       0x03: slave_store3,
       0x04: slave_store4,
    }	
    context = ModbusServerContext(slaves=slaves, single=False)
    
    # RTU Server
    StartSerialServer(context, port=PORT, framer=ModbusRtuFramer, baudrate=38400)


if __name__ == "__main__":
    run_server()
