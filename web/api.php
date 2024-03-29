<?php 
$GITLAB_API_KEY= "0183c450e36331faa7cfd704006e0f34";
$ABSTINENT_API_KEY = "aac69111be3000c6f7608a1924b6f460";

// just auto update test
if (isset($_SERVER['HTTP_X_GITLAB_EVENT']) && $_SERVER['HTTP_X_GITLAB_EVENT'] === 'Push Hook') {
    if (isset($_SERVER['HTTP_X_GITLAB_TOKEN'])){
        if ($_SERVER['HTTP_X_GITLAB_TOKEN'] == $GITLAB_API_KEY){
            update_from_git();
        } else {
            http_response_code(401);
        }
    }
} else {
    if (isset($_GET["apiKey"])) {
        if ($_GET["apiKey"] == $ABSTINENT_API_KEY) {
            save_new_data();
        } else {
            http_response_code(401);
        }
    } else {
        test_db();
    }
} 

function test_db(){
    require("database.php");
    $pdo = connect();
    $query = 'SELECT count(rotations) as "rotations", HOUR(timestamp) as "hour" FROM stats GROUP BY HOUR(timestamp)';
    $result = fetch_query($pdo, $query);
    foreach($result as $hour) {
        echo "Hodina: ".$hour['hour']."; metrů: ".round($hour['rotations']*0.3926990, 2).". <br>";
    }
}
function save_new_data(){

    $ROTATION_LENGTH = 0.4303982;
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
        "topSpeed" => $previous_data["topSpeed"],
        "topSpeedKm" => $previous_data["topSpeedKm"], 
        "topSpeedTimestamp" => $previous_data["topSpeedTimestamp"],
    );

    if ($previous_data["topSpeed"] < $current_data["currentSpeed"]) { //we have a new record boys!
        $current_data["topSpeed"] = round($current_speed, 2);
        $current_data["topSpeedKm"] = round($current_speed * 3.6, 2);    
        $current_data["topSpeedTimestamp"] = time();
    }

    file_put_contents('current_data.json', json_encode($current_data)); 
	
    require("database.php");
    $pdo = connect();
    $query = 'INSERT INTO stats (rotations) VALUES (:rotations)';
    fetch_query($pdo, $query, ["rotations" => $new_rotations]);
    http_response_code(201);
}

function update_from_git() {
    shell_exec('touch /srv/var/pls-update-git-abstinent');
    http_response_code(201);
}
