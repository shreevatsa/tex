console.log('Running include-all');

let d = document.getElementById('all');
const numPages = 1383;
function addPage(p) {
    // <div id="0001"><img src="svgs/tex-0001.svg"/></div>
    let div = document.createElement('div');
    let idStr = p.toString().padStart(4, '0'); 
    div.id = idStr;
    let img = document.createElement('img');
    img.src = "svgs/tex-" + idStr + ".svg";
    div.appendChild(img);
    d.appendChild(div);
    if (p < numPages) {
	window.setTimeout(function() { addPage(p + 1); }, 1);
    }
}
addPage(1);
