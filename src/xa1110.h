

#ifndef XA1110_H
#define XA1110_H

#define PMTK_API_SET_FIX_CTL_100_MILLIHERTZ "$PMTK300,10000,0,0,0,0*2C"  // Obtain fix once every 10 seconds, 100 millihertz.
#define PMTK_SET_NMEA_UPDATE_100_MILLIHERTZ "$PMTK220,10000*2F"          // Output NMEA sentence once every 10 seconds, 100 millihertz.
#define PMTK_SET_NMEA_OUTPUT_ZDA "$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0*29" // Set output to ZDA only
#define PMTK_SET_BAUD_115200 "$PMTK251,115200*1F"

#endif // XA1110_H