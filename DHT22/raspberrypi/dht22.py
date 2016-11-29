# Python code stored in dht22.py
import Adafruit_DHT as dht
humidity, temperature = dht.read_retry(dht.DHT22, 12)
print 'Temperature={0:0.1f}*C  Humidity={1:0.1f}%'.format(temperature, humidity)
