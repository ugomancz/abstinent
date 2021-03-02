<?php

$host = "localhost";
$user = "abstinent";
$password = "Lo3HvrJMJ1nfGMMt";
$dbname = "abstinent";
$dsn = "mysql:host=$host;dbname=$dbname;charset=utf8mb4;collation=utf8mb4_unicode_ci";
$options = [
  PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION,
  PDO::ATTR_DEFAULT_FETCH_MODE => PDO::FETCH_ASSOC,
  PDO::ATTR_EMULATE_PREPARES => false
];

try {$db = new PDO($dsn, $user, $password, $options); } 
catch (\PDOException $e) {throw new \PDOException($e->getMessage(), (int)$e->getCode());}


function run_query($query){
  global $db;
  $stmt = $db->query($query);
  return $stmt->rowCount();
  }

function fetch_query($query, $array = []){
  global $db;
  $questions_marks = implode(',', array_fill(0, count($array), '?'));
  $fixed_query = str_replace('#', $questions_marks, $query);
  $stmt = $db->prepare($fixed_query);
  $stmt->execute($array);
  $response = [];
  while ($row = $stmt->fetch()){
    $response[] = $row;
  }
  return $response;
}

$query = 'INSERT INTO stats(rotation) VALUES (:rotation)';
run_query($query, ["rotation" => $new_rotations]);

echo "ok?";