"use strict";
const token = document.getElementById("token");
const status = document.getElementById("status");
let ticker = null;
let active = false;
async function api(path, body={}) {
  const response = await fetch("/api/" + path, {
    method: "POST",
    headers: {"Content-Type": "application/json", "Authorization": "Bearer " + token.value},
    body: JSON.stringify(body),
    cache: "no-store"
  });
  const result = await response.json();
  if (!response.ok) throw new Error(result.error || "request failed");
  return result;
}
async function action(name, body={}) {
  try { const result=await api(name,body); status.textContent=result.message; }
  catch(e) {status.textContent=String(e.message || e);}
}
async function stopNow() {
  active=false;clearInterval(ticker);ticker=null;
  await action("disarm");
}
document.getElementById("stop").addEventListener("click", stopNow);
document.getElementById("arm").addEventListener("click", async () => {
  if (!confirm("Request STM32 ARM? Only after physical E-stop and wiring acceptance.")) return;
  await action("arm");
});
for (const button of document.querySelectorAll(".pad button")) {
  function press(event) {
    if (active) return;
    event.preventDefault();active=true;
    button.setPointerCapture(event.pointerId);
    const axes={forward:Number(button.dataset.f), lateral:Number(button.dataset.l), turn:Number(button.dataset.t)};
    action("drive", axes);
    ticker=setInterval(() => {if (active) action("drive",axes);}, 100);
  }
  button.addEventListener("pointerdown",press);
  button.addEventListener("pointerup",stopNow);
  button.addEventListener("pointercancel",stopNow);
  button.addEventListener("lostpointercapture",() => {if(active) stopNow();});
}
document.addEventListener("visibilitychange", () => {if (document.hidden) stopNow();});
window.addEventListener("pagehide", () => {active=false;clearInterval(ticker);navigator.sendBeacon /* no auth on beacon: MCU timeout remains decisive */;});
