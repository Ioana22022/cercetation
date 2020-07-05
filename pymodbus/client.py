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
#res = cli.read_input_registers(40001, count=2, unit=1)
#res1 = cli.read_input_registers(40002, count=1, unit=2)
#res3 = cli.read_input_registers(40005, count=2, unit=3)
#res2 = cli.read_input_registers(40004, count=1, unit=4)
rq = cli.write_register(40001, 261, unit=1)
rr = cli.read_holding_registers(40001, count=1, unit=1)

rq_fail = cli.write_register(40001, 255, unit=1)
rr_fail = cli.read_holding_registers(40001, count=1, unit=1)


cli.close()

#print(res1.registers)
print(rr.registers)
print(rr_fail.registers)
#print(res)
#print(res.registers)
#print(res3.registers)
#print(res2.registers)
