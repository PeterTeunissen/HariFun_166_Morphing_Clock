
User enters city name
User enters google api key

Use google api to get lat long for the location

https://maps.googleapis.com/maps/api/geocode/json?&address=quito&key=AIzaSyAVA_l4_iAv5Vm8i74xjTXA551EQybSP38

38.956571, -94.523947
-180.956571, -180.956571

Use google api to get the dst and timezone offsets

lat long of from previous call

```timestamp```: specifies the desired time as seconds since midnight, January 1, 1970 UTC. The Time Zone API uses the timestamp to determine whether or not Daylight Savings should be applied, based on the time zone of the location. Note that the API does not take historical time zones into account. That is, if you specify a past timestamp, the API does not take into account the possibility that the location was previously in a different time zone.

https://maps.googleapis.com/maps/api/timezone/json?location=40.117046,%20-75.541021&timestamp=1331161200&key=AIzaSyAVA_l4_iAv5Vm8i74xjTXA551EQybSP38

Weather API 


- Add weather icons
- Add DST lookup
- Remove leading zero on am/pm clock

maps.googleapis.com/maps/api/geocode/json?&address=quito&key=AIzaSyAVA_l4_iAv5Vm8i74xjTXA551EQybSP38


http://api.timezonedb.com/v2.1/get-time-zone?key=31VLCCL5BAKD&format=json&by=zone&zone=EST&time=18000

{
    "status": "OK",
    "message": "",
    "countryCode": "US",
    "countryName": "United States",
    "zoneName": "America/Indiana/Tell_City",
    "abbreviation": "EST",
    "gmtOffset": -18000,
    "dst": "0",
    "zoneStart": -5767200,
    "zoneEnd": 4422599,
    "nextAbbreviation": "EST",
    "timestamp": 0,
    "formatted": "1970-01-01 00:00:00"
}


String apiKey   = "aec6c8810510cce7b0ee8deca174c79a"; //e.g a hex string like "abcdef0123456789abcdef0123456789"
//the city you want the weather for 
String location = "Phoenixville,US"; //e.g. "Paris,FR"
char server[]   = "api.openweathermap.org";

http://api.openweathermap.org/data/2.5/weather?q=Phoenixville,PA&appid=aec6c8810510cce7b0ee8deca174c79a&cnt=1&units=metric
http://api.openweathermap.org/data/2.5/weather?q=Phoenixville,US&appid=aec6c8810510cce7b0ee8deca174c79a&cnt=1&units=imperial

  if (client.connect (server, 80))
  { 
    Serial.println ("connected."); 
    // Make a HTTP request: 
    client.print ("GET /data/2.5/weather?"); 
    client.print ("q="+location); 
    client.print ("&appid="+apiKey); 
    client.print ("&cnt=1"); 
    (*u_metric=='Y')?client.println ("&units=metric"):client.println ("&units=imperial");
    client.println ("Host: api.openweathermap.org"); 
    client.println ("Connection: close");
    client.println (); 

// Output
	
{
    "coord": {
        "lon": -75.52,
        "lat": 40.13
    },
    "weather": [
        {
            "id": 800,
            "main": "Clear",
            "description": "clear sky",
            "icon": "01n"
        }
    ],
    "base": "stations",
    "main": {
        "temp": -2.88,
        "pressure": 1034,
        "humidity": 49,
        "temp_min": -4,
        "temp_max": -1.7
    },
    "visibility": 16093,
    "wind": {
        "speed": 1.5,
        "deg": 190
    },
    "clouds": {
        "all": 1
    },
    "dt": 1548216900,
    "sys": {
        "type": 1,
        "id": 6134,
        "message": 0.0037,
        "country": "US",
        "sunrise": 1548245888,
        "sunset": 1548281408
    },
    "id": 5205849,
    "name": "Phoenixville",
    "cod": 200
}