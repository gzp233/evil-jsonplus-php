<?php

$arr = ['foo' => 1, 'bar' => 2, 'baz' => 3];
for ($i = 0; $i < 100; $i++) {
    echo json_encode($arr) . PHP_EOL;
}