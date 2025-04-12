const navbarPhone = document.querySelector(".options-phone");
const optionsPhone = document.querySelectorAll(".nav-option2");

var btn = document.querySelector(".btn");
btn.addEventListener("click", function () {
  if (this.classList.contains("active")) {
    this.classList.remove("active");
    this.classList.add("not-active");
    navbarPhone.style.display = "none";
  } else {
    this.classList.remove("not-active");
    this.classList.add("active");
    navbarPhone.style.display = "flex";
  }
});

optionsPhone.forEach((option) => {
  option.addEventListener("click", function () {
    console.log("atão");
    btn.classList.remove("active");
    btn.classList.add("not-active");
    navbarPhone.style.display = "none";
  });
});

document.addEventListener("DOMContentLoaded", function () {
  const buttons = document.querySelectorAll(".best-gallery-buttons button");
  const imageRows = document.querySelectorAll(
    ".best-gallery-images .best-gallery-image-row, .best-gallery-images .best-gallery-image-row-2"
  );
  function hideAllImageRows() {
    imageRows.forEach((row) => {
      row.style.display = "none";
    });
  }
  function showImageRow(year) {
    const selectedRow = document.getElementById(`gallery-${year}`);
    const selectedRow2 = document.getElementById(`gallery-${year}-2`);
    if (selectedRow) {
      selectedRow.style.display = "flex";
    }
    if (selectedRow2) {
      selectedRow2.style.display = "flex";
    }
  }
  hideAllImageRows();
  showImageRow("2023");
  buttons.forEach((button) => {
    if (button.getAttribute("data-year") === "2023") {
      button.classList.add("clicked");
    }
    button.addEventListener("click", function () {
      const year = this.getAttribute("data-year");
      hideAllImageRows();
      showImageRow(year);
      buttons.forEach((btn) => btn.classList.remove("clicked"));
      this.classList.add("clicked");
    });
  });
});

document.addEventListener("DOMContentLoaded", (event) => {
  const navbar = document.querySelector("header");
  const homepageLimit = document.querySelector(".home-page").offsetHeight;

  const onScroll = () => {
    const scrollPosition = window.scrollY + navbar.offsetHeight;

    if (scrollPosition > homepageLimit) {
      navbar.style.background = "white";
      navbar.style.boxShadow = "0px 1px 5px #0000006e";
    } else {
      navbar.style.background = "";
      navbar.style.boxShadow = "";
    }
  };

  window.addEventListener("scroll", onScroll);
});

document.addEventListener("DOMContentLoaded", function () {
  var leftBox = document.querySelector(".course-image-box-left");
  var rightBox = document.querySelector(".course-image-box-right");
  var leftBoxText = document.querySelector(".course-image-box-left h4");
  var rightBoxText = document.querySelector(".course-image-box-right h4");
  var leftBoxShadow = document.querySelector(".course-image-box-left-shadow");
  var rightBoxShadow = document.querySelector(".course-image-box-right-shadow");

  leftBox.addEventListener("mouseenter", function () {
    leftBox.style.width = "80%";
    rightBox.style.width = "20%";
    rightBoxText.style.display = "none";
    leftBoxShadow.style.display = "none";
  });

  leftBox.addEventListener("mouseleave", function () {
    leftBox.style.width = "50%";
    rightBox.style.width = "50%";
    rightBoxText.style.display = "block";
    leftBoxShadow.style.display = "block";
  });

  rightBox.addEventListener("mouseenter", function () {
    rightBox.style.width = "80%";
    leftBox.style.width = "20%";
    leftBoxText.style.display = "none";
    rightBoxShadow.style.display = "none";
  });

  rightBox.addEventListener("mouseleave", function () {
    rightBox.style.width = "50%";
    leftBox.style.width = "50%";
    leftBoxText.style.display = "block";
    rightBoxShadow.style.display = "block";
  });
});

//gallery
document.querySelectorAll(".best-gallery-images img").forEach((item) => {
  item.onclick = function () {
    document.getElementById("best-gallery-imageModal").style.display = "flex";
    document.getElementById("best-gallery-img01").src = this.src;
  };
});

document.getElementsByClassName("best-gallery-close")[0].onclick = function () {
  document.getElementById("best-gallery-imageModal").style.display = "none";
};

window.onclick = function (event) {
  if (event.target == document.getElementById("best-gallery-imageModal")) {
    document.getElementById("best-gallery-imageModal").style.display = "none";
  }
};
