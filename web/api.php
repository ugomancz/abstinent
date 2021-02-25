<?php 
$GITLAB_API_KEY= "0183c450e36331faa7cfd704006e0f34";
$ABSTINENT_API_KEY = "aac69111be3000c6f7608a1924b6f460";


if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    if (isset($_SERVER['HTTP_X_GITLAB_TOKEN'])){
        if ($_SERVER['X_GITLAB_TOKEN'] == $GITLAB_API_KEY){
            update_from_git();
        } else {
            http_response_code(401);
        }
    }
} else if ($_SERVER['REQUEST_METHOD'] === 'GET'){

    if (isset($_GET["apiKey"])) {
        if ($_GET["apiKey"] == $ABSTINENT_API_KEY) {
            save_new_data();
        } else {
            http_response_code(401);
        }
    }
} 

function save_new_data(){
    $ROTATION_LENGTH = 0.39269908;
    $previous_data = json_decode(file_get_contents('current_data.json'), true);

    $new_rotations = $_GET["newRotations"];
    $time_diff = isset($_GET["ms"]) ? $_GET["ms"] : 2000;
    $current_speed = $new_rotations / ($time_diff / 1000) * $ROTATION_LENGTH;
    $current_data = array(        
	    "totalRotations" => $previous_data["totalRotations"] + $new_rotations,
        "totalDistance" => floor($previous_data["totalDistance"]+($new_rotations * $ROTATION_LENGTH)),
        "currentSpeed" => round($current_speed, 2),
        "currentSpeedKm" => round($current_speed * 3.6, 2),
        "currentTimestamp" => time(),
    );

    $current_data["topSpeed"] = $previous_data["topSpeed"] < $current_data["currentSpeed"] ? $current_data["currentSpeed"] : $previous_data["topSpeed"];
    $current_data["topSpeedKm"] = round($current_data["topSpeed"] * 3.6, 2);    

    file_put_contents('current_data.json', json_encode($current_data));
    // Todo: save into CSV for logging
    http_response_code(201);
}

function update_from_git() {
    shell_exec('touch /srv/var/pls-update-git-abstinent');
    http_response_code(201);
}