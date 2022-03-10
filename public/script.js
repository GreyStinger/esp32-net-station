function toggle () {
    var form = document.createElement('form');
    form.setAttribute('method', 'post');
    form.setAttribute('action', '192.168.1.60/api/v1/gpio/toggle/');
    form.style.display = 'hidden';
    document.body.appendChild(form)
    form.submit();
}