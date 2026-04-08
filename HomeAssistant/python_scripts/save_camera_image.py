# Python script to save image payload from the esp-32 cam once motion is detected

# The payload comes from the automation's 'data:' section
payload = data.get("payload")

if payload:
    # Get timestamp without importing 'time' or 'datetime'
    now = hass.states.get("sensor.date_time_iso")
    if now:
        time_str = now.state.replace(":", "-")
    else:
        # Fallback if you don't have that sensor enabled
        time_str = "motion_detected"

    filename = "www/camera_{}.jpg".format(time_str)
    
    if isinstance(payload, str):
        payload = payload.encode()

    try:
        with open(filename, "wb") as f:
            f.write(payload)
        logger.info("ESP32-CAM: Successfully saved to " + filename)
    except Exception as e:
        logger.error("ESP32-CAM: Save failed: " + str(e))
else:
    logger.error("ESP32-CAM: No payload received.")