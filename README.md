# Доработка штатного БК Pajero Pinin / IO
Штатный бортовой комп Pinin/IO состоит из двух плат, процессорной платы и платы дисплея, данный проект описывает, что нужно сделать, чтобы отрезать плату дисплея и бахнуть вместо нее ардуину, которая будет выполнять роль дисплея. То есть считывать то, что должно быть отображено на дисплее (температуру, время, расход, запас топлива, среднюю скорость) и выводить это в последовательный порт, либо куда либо еще, тут уж зависит от вашей фантазии. **Внимание!** Эта хрень работает с дисплейной платой под управлением чипа **LC75874**. С другими чипами может работать а может и не работать, в любом случае это не займет много времени, чтобы переделать все на другой чип, так как я на 100% уверен что другие чипы используют схожий протокол.

Дисплей со всеми включенными сегментами:

![segments](https://github.com/angrycoding/pajero-pinin-io-lcd-slave/blob/master/all_segments.jpg)

Схема подключения ардуины и процессорной платы:

![segments](https://github.com/angrycoding/pajero-pinin-io-lcd-slave/blob/master/circuit.png)
