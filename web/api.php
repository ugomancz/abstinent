<?php 
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    print_r($_POST);
    // TODO: write data to current_data.json
    /* json example:
    {
        "totalDistance": 11050,
        "topSpeed": 100,
        "topSpeedKm": 50,
        "currentSpeed": 50,
        "currentSpeedKm": 18
    } 
     */
}