<?php
function connect() {
  $host = "89.221.222.94:3333";
  $user = "abstinent";
  $password = "Lo3HvrJMJ1nfGMMt";
  $dbname = "abstinent";
  $dsn = "mysql:host=$host;dbname=$dbname;charset=utf8mb4;collation=utf8mb4_unicode_ci";
  $options = [
    PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION,
    PDO::ATTR_DEFAULT_FETCH_MODE => PDO::FETCH_ASSOC,
    PDO::ATTR_EMULATE_PREPARES => false
  ];

  try {
	$pdo = new PDO($dsn, $user, $password, $options); 
  } 
  catch (\PDOException $e) {
	throw new \PDOException($e->getMessage(), (int)$e->getCode());
  }
  return $pdo;
}

function run_query($pdo, $query){
  $stmt = $pdo->query($query);
  return $stmt->rowCount();
}


function fetch_query($pdo, $query, $array = []){
  $questions_marks = implode(',', array_fill(0, count($array), '?'));
  $fixed_query = str_replace('#', $questions_marks, $query);
  $stmt = $pdo->prepare($fixed_query);
  $stmt->execute($array);
  $response = [];
  while ($row = $stmt->fetch()){
    $response[] = $row;
  }
  return $response;
}


