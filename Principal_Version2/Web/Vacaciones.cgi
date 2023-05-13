t <html><head><title>Servidor Control</title>
t <script language=JavaScript type="text/javascript" src="xml_http.js"></script>
t <script language=JavaScript type="text/javascript">
t  </script> </head>
i pg_header.inc
t <h2 align=center><br>Modulo del modo Vacaciones</h2>
t <p><font size="2">Esta página sirve entrar al modo <b> Vacaciones </b> o quedarse en el modo normal.
t <form action="Vacaciones.cgi" method="post" name="Vaciones">
t <input type="hidden" value="Vaca" name="pg">
t <p align=left>
t <form><h4>
t <input type="radio" id="ModoVaca" name=ModoVaca value = "1">
t <label for="ModoVaca">Entrar al modo vacaciones</label><br>
t <input type="radio" id="ModoVaca" name=ModoVaca value = "0">
t <label for="ModoVaca">Seguir en modo normal</label><br>
t </h4>
t <p align=center>
t <input type=submit name=set value="Enviar" id="valor">
t </p></form>
i pg_footer.inc
. End of script must be closed with period.