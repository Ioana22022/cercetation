from pymodbus.client.sync import ModbusSerialClient as ModbusClient
from pymodbus.transaction import ModbusRtuFramer

PORT='/dev/ttyUSB1'
#cli = ModbusClient(method='rtu', port='/dev/ttyUSB1', framer=ModbusRtuFramer, baudrate=38400)
cli = ModbusClient(method='rtu', port=PORT, baudrate=38400)
print(cli)

cli.connect()
res = cli.read_input_registers(40001, count=2, unit=1)
res1 = cli.read_input_registers(40002, count=2, unit=2)
#res2 = cli.read_input_registers(40003, count=1, unit=4)
cli.close()

print(res)
print(res.registers)
print(res1.registers)
#print(res2.registers)
