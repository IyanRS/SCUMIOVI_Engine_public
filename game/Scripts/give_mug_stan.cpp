if (e.player.getCurrentHorizontalDirection() == RIGHT)
    e.player.playAnimation("interactRight");
else
    e.player.playAnimation("interactLeft");
e.player.getScene()->getGameObject("stan")->say("Gracias");