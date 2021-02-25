<?php 
$GITLAB_TOKEN = "0183c450e36331faa7cfd704006e0f34";
$ROTATION_LENGTH 0.39269908;

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    if (isset($_SERVER['HTTP_X_GITLAB_TOKEN'])){
        if ($_SERVER['X_GITLAB_TOKEN'] == $GITLAB_TOKEN){
            update_from_git();
        } else {
            http_response_code(401);
        }
    }
} else if ($_SERVER['REQUEST_METHOD'] === 'GET'){
    save_new_data();
    /*
    if (isset($_GET["api_key"])) {
        if ($_GET["api_key"] != "change-it-later-to-equals") {
            save_new_data();
        } else {
            http_response_code(401);
        }
    }/*
} 

function save_new_data(){
    $previous_data = json_decode(file_get_contents('current_data.json'));

    $new_rotations = $_GET["newRotations"];
    $time_diff = isset($_GET["ms"]) ? $_GET["ms"] : 2000;
    $current_speed = $new_rotations / ($time_diff / 1000) * $ROTATION_LENGTH;
    $current_data = array(
        "totalRotations" => $previous_data["totalRotations"] + $new_rotations;
        "totalDistance" => $previous_data["totalDistance"]+($new_data * $ROTATION_LENGTH),
        "currentSpeed" => $current_speed ,
        "currentSpeedKm" => $current_speed * 3.6,
        "currentTimestamp" => time(),
    );
    if ($previous_data["topSpeed"] < $current_data["currentSpeed"]) {
        $current_data["topSpeed"] = $current_speed;
        $current_data["topSpeedKm"] = $current_speed * 3.6;
    }
    file_put_contents('current_data.json', json_encode($current_data));
    // Todo: save into CSV for logging
    http_response_code(201);
}

function update_from_git() {
    shell_exec('touch /srv/var/pls-update-git-abstinent');
    http_response_code(201);
}