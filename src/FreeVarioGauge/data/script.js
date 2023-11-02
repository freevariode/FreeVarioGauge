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

function renew() {
	fetch('/SoftwareVersion').then(function (response) {
		  return response.json();
	}).then(function (text) {
		  document.querySelector('#SoftwareVersion').innerHTML = text;
	});
}

document.addEventListener('DOMContentLoaded', renew);