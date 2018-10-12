/Remark Here are some sample Number Pad macros
/macro {kp0} {scan} {directions}
/macro {ckp0} {search} {directions}
/macro {kp2} {south} {directions}
/macro {akp2} {cast farsight south brief} {directions}
/macro {akp3} {cast farsight down brief} {directions}
/macro {kp3} {down} {directions}
/macro {kp4} {west} {directions}
/macro {akp4} {cast farsight west brief} {directions}
/macro {ckp5} {c cure blind} {keys}
/macro {akp5} {c remove poison} {keys}
/macro {kp6} {east} {directions}
/macro {akp6} {cast farsight east brief} {directions}
/macro {kp8} {north} {directions}
/macro {akp8} {cast farsight north brief} {directions}
/macro {akp9} {cast farsight up brief} {directions}
/macro {kp9} {up} {directions}
/macro {kpstar} {get all corpse} {keys}
/macro {F8} {mode xp} {keys}
/macro {aF8} {mode res} {keys}
/Remark ...
/Remark typing spellsounds will toggle the med spell sounds on and off
/alias {spellsounds}{setsound spells}{settings}
/substitutedepth 2
/autoexec {/session connect}
