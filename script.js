// --- Document Ready Function ---
document.addEventListener('DOMContentLoaded', function() {
    // Handle model loading
    setupModelLoader();
    
    // No longer needed since contact form is removed
    // setupContactForm();
    
    // Handle keyboard accessibility
    setupKeyboardNavigation();
    
    // Handle image lazy loading
    setupLazyLoading();
    
    // Add animation for page elements and initialize visible elements
    animateOnScroll();
    initializeVisibleElements();
});

// --- Back to Top Button Logic --- 
let backToTopButton = document.getElementById("back-to-top-btn");

// Improved scroll function with throttling
let scrollTimeout;
window.onscroll = function() {
    if (!scrollTimeout) {
        scrollTimeout = setTimeout(function() {
            scrollFunction();
            scrollTimeout = null;
        }, 100); // Throttle to improve performance
    }
};

function scrollFunction() {
    if (document.body.scrollTop > 200 || document.documentElement.scrollTop > 200) {
        backToTopButton?.classList.add("show");
    } else {
        backToTopButton?.classList.remove("show");
    }
    
    // Add animation to elements when they scroll into view
    const animElements = document.querySelectorAll('.animate-on-scroll:not(.animated)');
    animElements.forEach(element => {
        if (isElementInViewport(element)) {
            element.classList.add('animated');
        }
    });
}

// Check if an element is in viewport
function isElementInViewport(el) {
    const rect = el.getBoundingClientRect();
    return (
        rect.top <= (window.innerHeight || document.documentElement.clientHeight) &&
        rect.bottom >= 0
    );
}

// --- Modal Logic --- 
var modal = document.getElementById("imageModal");
var modalImg = document.getElementById("modalImage");
var modalCaption = document.getElementById("modalTitle");

// Trap focus within modal when open
function trapFocus(element) {
    const focusableElements = element.querySelectorAll('button, [href], input, select, textarea, [tabindex]:not([tabindex="-1"])');
    const firstFocusableElement = focusableElements[0];
    const lastFocusableElement = focusableElements[focusableElements.length - 1];
    
    // Set keydown event
    element.addEventListener('keydown', function(e) {
        const isTabPressed = e.key === 'Tab' || e.keyCode === 9;
        
        if (!isTabPressed) return;
        
        if (e.shiftKey) {
            // If shift key pressed for shift + tab combination
            if (document.activeElement === firstFocusableElement) {
                lastFocusableElement.focus(); // Move to last focusable element
                e.preventDefault();
            }
        } else {
            // If tab key is pressed without shift
            if (document.activeElement === lastFocusableElement) {
                firstFocusableElement.focus(); // Move to first focusable element
                e.preventDefault();
            }
        }
    });
}

// Improved modal open function
function openModal(imgSrc, altText) {
    if (modal && modalImg) {
        document.body.style.overflow = 'hidden'; // Prevent scrolling behind modal
        modal.style.display = "flex";
        modalImg.src = imgSrc;
        
        // Set alt text from image or parameter
        const alt = altText || event.target.alt || "Gallery image";
        modalImg.alt = alt;
        modalCaption.textContent = alt;
        
        // Set focus on close button
        setTimeout(function() {
            const closeBtn = modal.querySelector('.close-btn');
            closeBtn.focus();
        }, 100);
        
        // Trap focus in modal
        trapFocus(modal);
        
        // Announce to screen readers
        announceToScreenReader("Image opened in modal. Press Escape to close.");
    }
}

// Improved modal close function
function closeModal() {
    if (modal) {
        modal.style.display = "none";
        document.body.style.overflow = ''; // Restore scrolling
        
        // Return focus to the element that opened the modal
        if (window.lastFocusedElement) {
            window.lastFocusedElement.focus();
        }
        
        // Announce to screen readers
        announceToScreenReader("Modal closed.");
    }
}

// Announce messages to screen readers
function announceToScreenReader(message) {
    const announcement = document.createElement('div');
    announcement.setAttribute('aria-live', 'polite');
    announcement.setAttribute('aria-atomic', 'true');
    announcement.classList.add('sr-only');
    announcement.textContent = message;
    
    document.body.appendChild(announcement);
    setTimeout(() => {
        document.body.removeChild(announcement);
    }, 1000);
}

// Close modal with Escape key
document.addEventListener('keydown', function(e) {
    if (e.key === 'Escape' && modal.style.display === 'flex') {
        closeModal();
    }
});

// Store the element that opened the modal before opening it
document.addEventListener('click', function(e) {
    if (e.target.onclick && e.target.onclick.toString().includes('openModal')) {
        window.lastFocusedElement = e.target;
    }
});

// Optional: Close modal if clicking outside the image
if (modal) {
    modal.onclick = function(event) {
        if (event.target == modal) {
            closeModal();
        }
    }
}

// --- Masonry Layout for Gallery --- 
var galleryContainer = document.querySelector('.gallery-container');
if (galleryContainer) {
    // Initialize Masonry after all images have loaded
    imagesLoaded(galleryContainer, function() {
        var msnry = new Masonry(galleryContainer, {
            itemSelector: '.gallery-item',
            columnWidth: '.gallery-item',
            percentPosition: true,
            gutter: 14,
            horizontalOrder: true,
            fitWidth: true
        });
    });
}

// --- Model Loader Setup ---
function setupModelLoader() {
    const modelViewer = document.querySelector('model-viewer');
    const loadingContainer = document.querySelector('.loading-container');
    
    if (modelViewer && loadingContainer) {
        modelViewer.addEventListener('load', function() {
            // Hide loading animation when model is loaded
            loadingContainer.style.opacity = '0';
            setTimeout(() => {
                loadingContainer.style.display = 'none';
            }, 500);
            
            // Scale the model after loading
            // Access the model and apply scaling
            // const model = modelViewer.model;
            // if (model) {
            //     // Set model scale to 2 times its original size
            //     modelViewer.scale = '2 2 2'; // Adjust as needed
            //     // Force update of the model's transform
            //     modelViewer.updateFraming();
            // }
        });
        
        // Fallback if load event doesn't fire
        setTimeout(() => {
            loadingContainer.style.opacity = '0';
            setTimeout(() => {
                loadingContainer.style.display = 'none';
            }, 500);
        }, 8000);
    }
}

// --- Contact Form Setup --- (can be removed since we no longer have a contact form)
function setupContactForm() {
    // Function content removed since contact form is removed
}

// --- Keyboard Navigation Setup ---
function setupKeyboardNavigation() {
    // Make gallery images keyboard navigable
    const galleryImages = document.querySelectorAll('.gallery-item img');
    
    galleryImages.forEach(img => {
        img.setAttribute('tabindex', '0');
        img.addEventListener('keydown', function(e) {
            if (e.key === 'Enter' || e.key === ' ') {
                e.preventDefault();
                openModal(this.src, this.alt);
            }
        });
    });
}

// --- Lazy Loading Setup ---
function setupLazyLoading() {
    if ('loading' in HTMLImageElement.prototype) {
        // Native lazy loading supported
        const images = document.querySelectorAll('img[loading="lazy"]');
        images.forEach(img => {
            img.src = img.dataset.src;
        });
    } else {
        // Fallback for browsers that don't support native lazy loading
        const lazyLoadScript = document.createElement('script');
        lazyLoadScript.src = 'https://cdn.jsdelivr.net/npm/lozad/dist/lozad.min.js';
        document.body.appendChild(lazyLoadScript);
        
        lazyLoadScript.onload = function() {
            const observer = lozad('.lozad', {
                loaded: function(el) {
                    el.classList.add('loaded');
                }
            });
            observer.observe();
        }
    }
}

// --- Animate on Scroll ---
function animateOnScroll() {
    const elements = document.querySelectorAll('.team-card, .log-entry, .gallery-item');
    elements.forEach((el, index) => {
        el.classList.add('animate-on-scroll');
        el.style.transitionDelay = `${index * 0.1}s`;
    });
}

// Initialize elements that are visible on page load
function initializeVisibleElements() {
    // Make initially visible gallery items appear immediately
    const animElements = document.querySelectorAll('.animate-on-scroll');
    animElements.forEach(element => {
        if (isElementInViewport(element)) {
            element.classList.add('animated');
        }
    });
}

// --- Skip Link Focus Fix ---
window.addEventListener('hashchange', function() {
    if (location.hash === '#main-content') {
        const contentMain = document.getElementById('main-content');
        if (contentMain) {
            contentMain.setAttribute('tabindex', '-1');
            contentMain.focus();
        }
    }
});