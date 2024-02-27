<?php
$name = $_POST["input"];
if (empty($name) == true)
    $name = "not entered";

?>

<html lang="en">
<head>
    <title>Document</title>
</head>
<body>
    <p>name is :
        <?php echo $name ?>
    </p>
</body>
</html>