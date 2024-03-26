<?php

include_once "config.php";

function CheckUserAgent(){
    if ($_SERVER["HTTP_USER_AGENT"] != USER_AGENT){
        return false;
    }
    return true;
}

function CreatePath($cDir){
    if (!is_dir( $cDir )){
        mkdir($cDir, 0755,true);
    }
}

function HexToNumber($hexData){
    $unpacked = unpack("V", $hexData);
    return $unpacked[1];
}

if($_SERVER['REQUEST_METHOD'] != 'POST') {
    die;
}

if(!CheckUserAgent()){
    die;
}
$data = file_get_contents("php://input");

$hostnameSize = HexToNumber(substr($data, 0, 4));
$data = substr($data, 4);

$hostname = substr($data, 0, $hostnameSize);
$data = substr($data, $hostnameSize);

$savePath = SCREENSHOTS_FOLDER .'/'. $hostname . '/' . date('Y-m-d') . '/';
CreatePath($savePath);

file_put_contents($savePath . date('Y-m-d H_i_s') . '.jpg', $data);