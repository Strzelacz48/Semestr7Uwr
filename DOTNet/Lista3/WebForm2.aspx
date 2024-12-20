<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="WebForm2.aspx.cs" Inherits="Lista3App.WebForm2" %>

<!DOCTYPE html>

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
    <title></title>
</head>
<body>
    <form id="form1" runat="server">
        <div>
            <input type="submit" value="get" onclick="return myFunction()"/>
            <a onclick="document.getElementById('form1').submit();">post</a>
        </div>
    </form>
   <script>
function myFunction() {
    location.href = "WebForm1.aspx";
    return false;
}
   </script>
</body>
</html>