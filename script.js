let slideIndex = 0;
showSlides(slideIndex);

function showSlides(n) {
    let slides = document.getElementsByClassName("carousel-image");
    if (n >= slides.length) { slideIndex = 0 }
    if (n < 0) { slideIndex = slides.length - 1 }
    for (let i = 0; i < slides.length; i++) {
        slides[i].style.display = "none";
    }
    slides[slideIndex].style.display = "block";
}

document.querySelector(".prev").addEventListener("click", () => {
    showSlides(--slideIndex);
});

document.querySelector(".next").addEventListener("click", () => {
    showSlides(++slideIndex);
});
