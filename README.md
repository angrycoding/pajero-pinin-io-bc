## Проект бортового компьютера для Mitsubishi Pajero Pinin / IO на базе штатного БК
Хотелось бы сказать спасибо человеку который сделал это: http://carisma-club.su/index.php?showtopic=2770 за то, что он это сделал, а также за то, что все таки **не дал** мне прошивку, благодаря ему, мне пришлось разбираться с этим самому, что несомненно пошло мне только на пользу. Здесь в одном месте собрана вся информация, о БК которую мне удалось накопать из различных источников.

Проект состоит из двух частей, каждая из которых может быть собрана отдельно. **Первая часть** - это взаимодействие штатного БК и Arduino, дело в том, что штатный БК может обрабатывать и выводить показания с датчиков, которые невозможно прочесть в K-линии (датчик забортной температуры и датчик уровня топлива), кроме того, штатный БК умеет интегрировать показания с других датчиков (датчик скорости), для оценки пройденного расстояния и вычисления среднего расхода (в отличие от мгновенного расхода, который может быть вычислен исходя из времени открытия форсунок), средней скорости и остатка топлива в километрах. На мой взгляд это очень ценная информация, поэтому я и решил использовать штатный БК, как одну из частей своей поделки. **Вторая часть** - это считывание всего того, что можно прочитать из т. н. K-линии, шины по которой можно достучаться до электронного блока управления (ЭБУ) двигателем. Там очень много различных параметров (как полезных так и не очень). Так что если вам не нужна одна из частей, можете просто отключить ее из схемы, убрать из прошивки соответствующую часть и использовать только то, что нужно вам.

- [Все о штатном БК](https://github.com/angrycoding/pajero-pinin-io-bc/wiki/%D0%92%D1%81%D0%B5-%D0%BE-%D1%88%D1%82%D0%B0%D1%82%D0%BD%D0%BE%D0%BC-%D0%91%D0%9A)
- [Обновляемый список PIDов для получения информации с ЭБУ](https://github.com/angrycoding/pajero-pinin-io-bc/wiki/MUT-%D0%B7%D0%B0%D0%BF%D1%80%D0%BE%D1%81%D1%8B)

Схема подключения:

![ожидание](https://raw.githubusercontent.com/angrycoding/pajero-pinin-io-bc/master/docs/circuit.png)

## TODO

Посмотреть как приделать эту штуку к Android - приложению: https://github.com/Prosjekt2-09arduino/STK500-Android для того, чтобы можно было бы обновлять прошивку через приложение.
