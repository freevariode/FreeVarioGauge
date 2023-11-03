function renew() {
	fetch('/SoftwareVersion').then(function (response) {
		  return response.json();
	}).then(function (text) {
		  document.querySelector('#SoftwareVersion').innerHTML = text;
	});
}

document.addEventListener('DOMContentLoaded', renew);

class SubmitBar {
	constructor(elem){
		this.elem = elem;
		this.percentField = this.elem.querySelector('.percent');
		this.status = 'save';
		this.progress = 0;
	}

	updateProgress(progress) {
		this.progress = progress;
		this.elem.setAttribute('data-progress', progress);
		let percentage = Math.round(progress * 100);
		this.percentField.innerHTML = percentage;
		this.elem.style.setProperty('--progress', percentage + '%');
	}

	reset() {
		this.updateProgress(0);
		this.status = 'save';
		this.elem.setAttribute('data-status', 'save');
	}

	start() {
		this.status = 'saving';
		this.elem.setAttribute('data-status', 'saving');
	}

	succeed() {
		this.updateProgress(1);
		this.status = 'completed';
		this.elem.setAttribute('data-status', 'completed');
	}

	fail() {
		this.status = 'failed';
		this.elem.setAttribute('data-status', 'failed');
	}
}


for(let form of document.querySelectorAll('form.background')){
	let submitBar = new SubmitBar(form.querySelector('.submitbar'));

	form.addEventListener('submit', (ev) => {
		ev.preventDefault();

		let data = new FormData(form);

		let request = new XMLHttpRequest();
		
		request.onreadystatechange = () => {
			if(request.readyState === 1){
				submitBar.start();
			} else if(request.readyState === 4){
				if(request.status === 200){
					submitBar.succeed();
					// form.reset();
				} else {
					submitBar.fail();
					console.error(request);
				}
			}
		}

		request.upload.onprogress = (e) => {
			if(e.lengthComputable){
				let progress = e.loaded / e.total;

				submitBar.updateProgress(progress);
			}
		}
		
		request.open('POST', form.action);
		request.send(data);
	});

	for(let elem of form.elements){
		if(elem instanceof HTMLInputElement || elem instanceof HTMLSelectElement || elem instanceof HTMLTextAreaElement){
			elem.addEventListener('input', () => {
				if(submitBar.status === 'completed' || submitBar.status === 'failed'){
					submitBar.reset();
				}
			});
		}
	}
}


// FLASH CYCLE SIMULATION

let elemCycledemo = document.getElementById('cycledemo');
let cycledemoRunning = false;
let overhanging = null;
let flashduration = 15;
let defaultCycleDuration = 15 * 4 + 600 + 300 + 100 + 1100;
let inputs = elemCycledemo.querySelectorAll('.break input');

if(elemCycledemo){
	function updateEnergy() {
		let onduration = flashduration * 4;
		let offduration = 0;

		for(inp of inputs){
			offduration += Number(inp.value) ?? 0;
		}


		let cycleDuration = onduration + offduration;

		let defaultOnRatio = onduration / defaultCycleDuration;
		let onRatio = onduration / cycleDuration;

		let energyusage = Math.round(onRatio / defaultOnRatio * 100);

		document.getElementById('heatpercent').innerHTML = energyusage + ' %';

		if(energyusage <= 100){
			document.getElementById('energy').setAttribute('data-level', 'green');
		} else if(energyusage <= 120){
			document.getElementById('energy').setAttribute('data-level', 'yellow');
		} else {
			document.getElementById('energy').setAttribute('data-level', 'red');
		}
	}

	for(inp of inputs){
		inp.addEventListener('input', () => { updateEnergy(); });
	}

	updateEnergy();

	async function runCycleDemo() {
		cycledemoRunning = true;
		elemCycledemo.classList.add('running');
		
		let flashes = elemCycledemo.getElementsByClassName('flash');
		let breaks = elemCycledemo.getElementsByClassName('breakbar');
		let flashtime = 300;

		while(cycledemoRunning){
			for(let i = 0; i < Math.max(flashes.length, breaks.length); i++){
				if(flashes[i]){
					await flash(flashes[i], flashduration, flashtime, true);
				}

				// await flashAll(flashes, flashtime);

				if(breaks[i] && inputs[i]){
					await flash(breaks[i], inputs[i].value);
				}
			}
		}
	}

	function flashAll(elems, duration) {
		for(elem of elems){
			elem.classList.add('current');
		}

		return new Promise((resolve) => {
			setTimeout(() => {
				for(elem of elems){
					elem.classList.remove('current');
				}

				resolve();
			}, duration);
		});
	}

	function flash(elem, duration, overhang = 0, endOverhang = false) {
		if(overhanging && endOverhang){
			overhanging.classList.remove('current');
			overhanging = null;
		}

		elem.classList.add('current');

		if(overhang){
			overhanging = elem;
		}

		return new Promise((resolve) => {
			setTimeout(() => {
				setTimeout(() => {
					elem.classList.remove('current');
					currentflashing = null;
				}, overhang);

				resolve();
			}, duration);
		});
	}

	function stopCycleDemo() {
		elemCycledemo.classList.remove('running');
		cycledemoRunning = false;
	}

	document.getElementById('cycletrystart').addEventListener('click', () => {
		runCycleDemo();
	});

	document.getElementById('cycletrystop').addEventListener('click', () => {
		stopCycleDemo();
	});
}

for(let inp of document.querySelectorAll('.fileinput input')){
	inp.addEventListener('change', () => {
		let name = inp.parentNode.querySelector('.filename');
		name.innerHTML = inp.files[0].name;
	});
}



