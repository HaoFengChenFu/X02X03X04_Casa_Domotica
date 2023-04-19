t <html><head><title>Servidor Control</title>
t <script language=JavaScript type="text/javascript" src="xml_http.js"></script>
t <script language=JavaScript type="text/javascript">
t  </script> </head>
i pg_header.inc
t <h2 align=center><br>Modulo del Ventilador</h2>
t <p><font size="2">Esta página sirve forzar el encendido o el apagado del <b> Ventilador </b>.
t <form action="Ventilador.cgi" method="post" name="Ventilador">
t <input type="hidden" value="Vent" name="pg">
t <table border=0 width=99%><font size="3">
t <tr bgcolor=#ffffff>
t  <th width=100%>Configuracion del Ventilador </th> </tr>
t <td align="center"><input type="text" readonly style="background-color: transparent; border: 0px" /td>
t <p align=left>
t <tr>
t <td align="center">
t <form>
t <label for="prueba">Elige el umbral de la temperatura (Entre 10 y 100 ºC):</label>
t <input type="number" id="UmbralTemp" name="UmbralTemp" min="10" max="100"><br>
t <input type="radio" id="VentEnable" name=VentHab value = "1">
t <label for="VentEnable">Encender Ventilador</label><br>
t <input type="radio" id="VentDisable" name=VentHab value = "0">
t <label for="VentDisable">Apagar Ventilador</label><br>
t </form>
t </font></table>
t <p align=center>
t <input type=submit name=set value="Enviar" id="valor">
t </p></form>
i pg_footer.inc
. End of script must be closed with period.