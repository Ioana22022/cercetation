from pymodbus.client.sync import ModbusSerialClient as ModbusClient
from pymodbus.transaction import ModbusRtuFramer

import logging
FORMAT = ('%(asctime)-15s %(threadName)-15s'
          ' %(levelname)-8s %(module)-15s:%(lineno)-8s %(message)s')
logging.basicConfig(format=FORMAT)
log = logging.getLogger()
log.setLevel(logging.DEBUG)

PORT='/dev/ttyUSB1'
cli = ModbusClient(method='rtu', port=PORT, baudrate=38400)
print(cli)

cli.connect()
res = cli.read_input_registers(40001, count=2, unit=1)
res1 = cli.read_input_registers(40002, count=2, unit=2)
res2 = cli.read_input_registers(40003, count=1, unit=4)
cli.close()

print(res)
print(res.registers)
print(res1.registers)
print(res2.registers)
