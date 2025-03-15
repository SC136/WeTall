let slideIndex = 0;
showSlides(slideIndex);

function showSlides(n) {
    let slides = document.getElementsByClassName("carousel-image");
    if (n >= slides.length) { slideIndex = 0 }
    if (n < 0) { slideIndex = slides.length - 1 }
    for (let i = 0; i < slides.length; i++) {
        slides[i].classList.remove("active");
    }
    slides[slideIndex].classList.add("active");
}

document.querySelector(".prev").addEventListener("click", () => {
    console.log("Previous button clicked");
    showSlides(--slideIndex);
});

document.querySelector(".next").addEventListener("click", () => {
    console.log("Next button clicked");
    showSlides(++slideIndex);
});
