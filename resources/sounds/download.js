const url = "https://cdn.freesound.org/previews/707/707041_9658839-lq.mp3";
const response = await fetch(url);
const blob = await response.blob();

const a = document.createElement("a");
a.href = URL.createObjectURL(blob);
a.download = "sound.mp3"; // Nome do arquivo ao salvar
document.body.appendChild(a);
a.click();
document.body.removeChild(a);