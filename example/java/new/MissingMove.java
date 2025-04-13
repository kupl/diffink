public class MissingMove {
  public void foo() {
    if (this.selected == null || this.selected.equals(opt.getOpt())) {
      this.selected = opt.getOpt();
    }
  }
}