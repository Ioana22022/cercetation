from pymodbus.client.sync import ModbusSerialClient as ModbusClient
from pymodbus.transaction import ModbusRtuFramer

#cli = ModbusClient(method='rtu', port='/dev/ttyUSB1', framer=ModbusRtuFramer, baudrate=38400)
cli = ModbusClient(method='rtu', port='/dev/ttyUSB1', baudrate=38400)
print(cli)

cli.connect()
res = cli.read_input_registers(40001, count=2, unit=1)
cli.close()

print(res.registers)
